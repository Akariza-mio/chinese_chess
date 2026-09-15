#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>

#include "network_transport.hpp"

#include <array>
#include <atomic>
#include <climits>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <utility>

namespace {

constexpr std::size_t receive_buffer_size = 4096;
constexpr std::size_t maximum_pending_text = 64 * 1024;

std::string winsock_error_text(std::string_view action) {
    return std::string(action) + " failed, WSA error " +
        std::to_string(WSAGetLastError());
}

} // namespace

class network_transport::implementation {
public:
    implementation() {
        WSADATA data{};
        winsock_ready_ = WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }

    ~implementation() {
        stop();
        if (winsock_ready_) WSACleanup();
    }

    bool start_host(std::uint16_t port, std::string& error) {
        if (!prepare_start(error)) return false;
        state_ = network_connection_state::hosting;
        worker_ = std::thread([this, port] { host_worker(port); });
        return true;
    }

    bool start_client(
        std::string_view ipv4_address,
        std::uint16_t port,
        std::string& error
    ) {
        if (!prepare_start(error)) return false;
        if (ipv4_address.empty()) {
            error = "IPv4 address cannot be empty";
            return false;
        }

        state_ = network_connection_state::connecting;
        const std::string address(ipv4_address);
        worker_ = std::thread([this, address, port] {
            client_worker(address, port);
        });
        return true;
    }

    bool send_text(std::string_view text, std::string& error) {
        error.clear();
        if (text.empty()) {
            error = "cannot send empty text";
            return false;
        }
        if (state_ != network_connection_state::connected) {
            error = "network peer is not connected";
            return false;
        }

        std::scoped_lock send_lock(send_mutex_);
        std::scoped_lock socket_lock(socket_mutex_);
        if (peer_socket_ == INVALID_SOCKET) {
            error = "network socket is closed";
            return false;
        }

        std::size_t sent_total = 0;
        while (sent_total < text.size()) {
            const std::size_t remaining = text.size() - sent_total;
            const int chunk_size = remaining > static_cast<std::size_t>(INT_MAX)
                ? INT_MAX
                : static_cast<int>(remaining);
            const int sent = send(
                peer_socket_, text.data() + sent_total, chunk_size, 0
            );
            if (sent == SOCKET_ERROR || sent == 0) {
                error = winsock_error_text("send");
                return false;
            }
            sent_total += static_cast<std::size_t>(sent);
        }
        return true;
    }

    std::vector<network_event> take_events() {
        std::scoped_lock lock(event_mutex_);
        std::vector<network_event> result;
        result.reserve(events_.size());
        while (!events_.empty()) {
            result.push_back(std::move(events_.front()));
            events_.pop_front();
        }
        return result;
    }

    network_connection_state state() const { return state_.load(); }
    bool winsock_ready() const { return winsock_ready_; }

    void stop() {
        stop_requested_ = true;
        close_all_sockets();
        if (worker_.joinable()) worker_.join();
        state_ = network_connection_state::disconnected;
    }

private:
    bool prepare_start(std::string& error) {
        error.clear();
        if (!winsock_ready_) {
            error = "WSAStartup failed";
            return false;
        }
        if (worker_.joinable() ||
            state_ != network_connection_state::disconnected) {
            error = "network transport is already running";
            return false;
        }

        stop_requested_ = false;
        std::scoped_lock lock(event_mutex_);
        events_.clear();
        return true;
    }

    void push_event(network_event_type type, std::string text = {}) {
        std::scoped_lock lock(event_mutex_);
        events_.push_back(network_event{ type, std::move(text) });
    }

    void fail(std::string message) {
        if (stop_requested_) return;
        state_ = network_connection_state::failed;
        push_event(network_event_type::error, std::move(message));
    }

    bool publish_listen_socket(SOCKET socket_value) {
        std::scoped_lock lock(socket_mutex_);
        if (stop_requested_) return false;
        listen_socket_ = socket_value;
        return true;
    }

    bool publish_peer_socket(SOCKET socket_value) {
        std::scoped_lock lock(socket_mutex_);
        if (stop_requested_) return false;
        peer_socket_ = socket_value;
        return true;
    }

    void close_listen_socket_if(SOCKET expected) {
        std::scoped_lock lock(socket_mutex_);
        if (listen_socket_ == expected) {
            closesocket(listen_socket_);
            listen_socket_ = INVALID_SOCKET;
        }
    }

    void close_peer_socket_if(SOCKET expected) {
        std::scoped_lock lock(socket_mutex_);
        if (peer_socket_ == expected) {
            shutdown(peer_socket_, SD_BOTH);
            closesocket(peer_socket_);
            peer_socket_ = INVALID_SOCKET;
        }
    }

    void close_all_sockets() {
        std::scoped_lock lock(socket_mutex_);
        if (listen_socket_ != INVALID_SOCKET) {
            closesocket(listen_socket_);
            listen_socket_ = INVALID_SOCKET;
        }
        if (peer_socket_ != INVALID_SOCKET) {
            shutdown(peer_socket_, SD_BOTH);
            closesocket(peer_socket_);
            peer_socket_ = INVALID_SOCKET;
        }
    }

    void host_worker(std::uint16_t port) {
        const SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listener == INVALID_SOCKET) {
            fail(winsock_error_text("socket"));
            return;
        }
        if (!publish_listen_socket(listener)) {
            closesocket(listener);
            return;
        }

        BOOL reuse_address = TRUE;
        setsockopt(
            listener, SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<const char*>(&reuse_address),
            sizeof(reuse_address)
        );

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(port);
        if (bind(listener, reinterpret_cast<const sockaddr*>(&address),
            sizeof(address)) == SOCKET_ERROR) {
            fail(winsock_error_text("bind"));
            close_listen_socket_if(listener);
            return;
        }
        if (listen(listener, 1) == SOCKET_ERROR) {
            fail(winsock_error_text("listen"));
            close_listen_socket_if(listener);
            return;
        }

        const SOCKET peer = accept(listener, nullptr, nullptr);
        close_listen_socket_if(listener);
        if (peer == INVALID_SOCKET) {
            if (!stop_requested_) fail(winsock_error_text("accept"));
            return;
        }
        if (!publish_peer_socket(peer)) {
            closesocket(peer);
            return;
        }

        state_ = network_connection_state::connected;
        push_event(network_event_type::connected);
        receive_loop(peer);
        close_peer_socket_if(peer);
    }

    void client_worker(const std::string& ipv4_address, std::uint16_t port) {
        const SOCKET peer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (peer == INVALID_SOCKET) {
            fail(winsock_error_text("socket"));
            return;
        }
        if (!publish_peer_socket(peer)) {
            closesocket(peer);
            return;
        }

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        if (inet_pton(AF_INET, ipv4_address.c_str(), &address.sin_addr) != 1) {
            fail("invalid IPv4 address");
            close_peer_socket_if(peer);
            return;
        }
        if (connect(peer, reinterpret_cast<const sockaddr*>(&address),
            sizeof(address)) == SOCKET_ERROR) {
            if (!stop_requested_) fail(winsock_error_text("connect"));
            close_peer_socket_if(peer);
            return;
        }

        state_ = network_connection_state::connected;
        push_event(network_event_type::connected);
        receive_loop(peer);
        close_peer_socket_if(peer);
    }

    void receive_loop(SOCKET peer) {
        std::array<char, receive_buffer_size> buffer{};
        std::string pending;

        while (!stop_requested_) {
            const int received = recv(
                peer, buffer.data(), static_cast<int>(buffer.size()), 0
            );
            if (received == 0) {
                if (!stop_requested_) {
                    state_ = network_connection_state::disconnected;
                    push_event(
                        network_event_type::disconnected,
                        "the other player disconnected"
                    );
                }
                return;
            }
            if (received == SOCKET_ERROR) {
                if (!stop_requested_) fail(winsock_error_text("recv"));
                return;
            }

            pending.append(buffer.data(), static_cast<std::size_t>(received));
            if (pending.size() > maximum_pending_text &&
                pending.find('\n') == std::string::npos) {
                fail("received message is too long");
                return;
            }

            std::size_t newline = 0;
            while ((newline = pending.find('\n')) != std::string::npos) {
                if (newline > maximum_pending_text) {
                    fail("received message is too long");
                    return;
                }
                std::string line = pending.substr(0, newline);
                pending.erase(0, newline + 1);
                push_event(network_event_type::line_received, std::move(line));
            }
        }
    }

    bool winsock_ready_ = false;
    std::atomic<network_connection_state> state_{
        network_connection_state::disconnected
    };
    std::atomic<bool> stop_requested_ = false;

    mutable std::mutex socket_mutex_;
    SOCKET listen_socket_ = INVALID_SOCKET;
    SOCKET peer_socket_ = INVALID_SOCKET;
    std::mutex send_mutex_;
    std::mutex event_mutex_;
    std::deque<network_event> events_;
    std::thread worker_;
};

network_transport::network_transport()
    : impl_(std::make_unique<implementation>()) {}

network_transport::~network_transport() = default;

bool network_transport::start_host(std::uint16_t port, std::string& error) {
    return impl_->start_host(port, error);
}

bool network_transport::start_client(
    std::string_view ipv4_address,
    std::uint16_t port,
    std::string& error
) {
    return impl_->start_client(ipv4_address, port, error);
}

bool network_transport::send_text(std::string_view text, std::string& error) {
    return impl_->send_text(text, error);
}

std::vector<network_event> network_transport::take_events() {
    return impl_->take_events();
}

network_connection_state network_transport::state() const {
    return impl_->state();
}

bool network_transport::winsock_ready() const {
    return impl_->winsock_ready();
}

void network_transport::stop() {
    impl_->stop();
}

std::vector<std::string> get_local_ipv4_addresses() {
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return {};

    std::vector<std::string> result;
    std::unordered_set<std::string> seen;
    std::array<char, 256> hostname{};
    if (gethostname(hostname.data(), static_cast<int>(hostname.size())) == 0) {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        addrinfo* addresses = nullptr;
        if (getaddrinfo(hostname.data(), nullptr, &hints, &addresses) == 0) {
            for (addrinfo* current = addresses;
                current != nullptr;
                current = current->ai_next) {
                const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(
                    current->ai_addr
                );
                std::array<char, INET_ADDRSTRLEN> text{};
                if (inet_ntop(AF_INET, &ipv4->sin_addr,
                    text.data(), static_cast<DWORD>(text.size())) == nullptr) {
                    continue;
                }
                const std::string address(text.data());
                if (address != "127.0.0.1" && seen.insert(address).second) {
                    result.push_back(address);
                }
            }
            freeaddrinfo(addresses);
        }
    }

    WSACleanup();
    return result;
}
