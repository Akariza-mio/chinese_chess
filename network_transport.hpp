#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

constexpr std::uint16_t default_chess_port = 27015;

enum class network_connection_state {
    disconnected,
    hosting,
    connecting,
    connected,
    failed
};

enum class network_event_type {
    connected,
    line_received,
    disconnected,
    error
};

struct network_event {
    network_event_type type;
    std::string text;
};

class network_transport {
public:
    network_transport();
    ~network_transport();

    network_transport(const network_transport&) = delete;
    network_transport& operator=(const network_transport&) = delete;
    network_transport(network_transport&&) = delete;
    network_transport& operator=(network_transport&&) = delete;

    // 两个启动函数都会立即返回；监听、接受和连接在后台线程执行。
    bool start_host(std::uint16_t port, std::string& error);
    bool start_client(
        std::string_view ipv4_address,
        std::uint16_t port,
        std::string& error
    );

    // text 可以是一条或多条消息；完整消息必须以 '\n' 结尾。
    bool send_text(std::string_view text, std::string& error);
    std::vector<network_event> take_events();

    network_connection_state state() const;
    bool winsock_ready() const;
    void stop();

private:
    class implementation;
    std::unique_ptr<implementation> impl_;
};

// 返回适合展示给同一局域网玩家的本机 IPv4 地址，不包含 127.0.0.1。
std::vector<std::string> get_local_ipv4_addresses();
