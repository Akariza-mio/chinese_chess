# 中国象棋

一个使用 C++20 和 [raylib](https://www.raylib.com/) 开发的 Windows 中国象棋游戏。项目支持同机双人对局和局域网双人联机，包含完整走子判定、将军与胜负判断、棋盘视角翻转、移动动画和协商悔棋。

## 功能

- 中国象棋棋盘、棋子和中文界面绘制
- 车、马、炮、象、士、将、兵等完整走子与吃子规则
- 将帅照面、将军、自陷将军、将死和困毙判断
- 本地双人对局：两名玩家在同一台电脑上轮流操作
- 局域网联机对局：主机为红方，加入者为黑方
- 黑方视角自动将棋盘中心对称翻转
- 棋子移动和吃子动画
- 联机悔棋申请：对方可以同意或拒绝
- 对局结束后支持重新开始

> 本项目目前没有人机 AI。“本地对局”指同机双人轮流走棋，且不支持悔棋；悔棋功能仅用于局域网联机。

## 开发环境

当前项目使用以下环境开发和验证：

- 操作系统：Windows 11
- IDE：Visual Studio 2026
- 工具集：MSVC v145
- C++ 标准：C++20
- Windows SDK：10.0
- 图形库：raylib（动态库随项目提供）
- 网络接口：Windows Winsock 2
- 源文件编码：UTF-8

依赖已经放在仓库的 `third_party/` 和 `assets/` 中，无需另外下载 raylib 或中文字体。

## 运行环境

发布版面向 Windows x64，建议使用 Windows 10 或 Windows 11。

运行发布包不需要安装：

- Visual Studio
- C++ 编译器
- raylib 开发环境
- Visual C++ Redistributable

发布目录中必须保留以下内容：

```text
ChineseChess.exe
raylib.dll
assets/
└── fonts/
    ├── NotoSansCJKsc-Regular.otf
    └── LICENSE.txt
```

## 下载与运行

1. 在仓库的 **Releases** 页面下载 `ChineseChess-Windows-x64.zip`。
2. 完整解压压缩包，不要直接在压缩包预览窗口中运行程序。
3. 双击 `ChineseChess.exe`。

程序未经商业代码签名，Windows SmartScreen 可能显示“未知发布者”。请仅运行来自可信来源的构建。

## 从源码构建

1. 使用 Visual Studio 2026 打开 `chess.slnx`。
2. 选择 `x64` 平台。
3. 开发调试选择 `Debug`，生成发布版选择 `Release`。
4. 执行“生成解决方案”。

项目文件会从以下相对路径读取依赖：

```text
third_party/raylib/include
third_party/raylib/lib
third_party/raylib/bin/raylib.dll
assets/fonts/NotoSansCJKsc-Regular.otf
```

构建后，MSBuild 会自动把 `raylib.dll` 复制到输出目录。程序运行时的工作目录应为项目根目录，以便读取 `assets/fonts/`。

## 打包与部署

在项目根目录双击：

```text
package_release.bat
```

或者在 PowerShell 中执行：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\package_release.ps1
```

脚本会完成以下操作：

1. 构建 `Release | x64`。
2. 收集 EXE、raylib DLL、字体和许可证。
3. 生成可直接分发的压缩包：

```text
dist/ChineseChess-Windows-x64.zip
```

`package_release.ps1` 默认使用本项目开发机器上的 MSBuild 路径。如果 Visual Studio 安装在其他位置，可手动指定：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File .\package_release.ps1 `
  -MsBuildPath "C:\Path\To\MSBuild.exe"
```

`dist/` 是生成目录，已被 `.gitignore` 排除。发布成品应上传到 GitHub Release 的 Assets，而不是提交到源码仓库。

## 局域网联机

联机使用 TCP，默认端口为 `27015`。主机负责验证走法并广播权威棋局状态。

1. 两台电脑连接同一个 Wi-Fi 或局域网。
2. 红方点击“创建联机”。
3. 红方把界面显示的局域网 IPv4 地址告诉黑方。
4. 黑方点击“加入联机”，输入该地址并按回车。
5. Windows 防火墙首次询问时，主机需要允许程序访问“专用网络”。

联机双方必须使用相同版本。当前网络协议版本为 3，旧版客户端不能与新版客户端联机。

### 联机悔棋

- 已经走过棋的一方可以点击“悔棋”。
- 对方会收到确认窗口，并选择“同意”或“拒绝”。
- 同意后，棋盘恢复到申请方最近一次走棋之前。
- 如果对方在该步之后已经走棋，这些后续走法也会被撤销。
- 等待回应期间双方暂停走棋。

## 操作方式

- 鼠标左键点击本方棋子进行选择。
- 再点击目标交叉点完成走棋。
- 不符合规则的走法不会执行。
- 联机模式右上角提供“悔棋”和“返回菜单”按钮。
- 对局结束后点击“重开”开始新棋局。

## 代码结构

| 文件或目录 | 作用 |
| --- | --- |
| `main.cpp` | 程序入口、页面状态、输入处理和主循环 |
| `chess_types.hpp` | 阵营、棋子类型、棋局状态、坐标和走法等基础类型 |
| `chess_board.hpp/.cpp` | 10×9 棋盘数据、棋子访问、移动和初始布局 |
| `rule_engine.hpp/.cpp` | 走子合法性、将军、将死、困毙和合法着法判断 |
| `game_state.hpp/.cpp` | 当前棋盘、行棋方、胜负状态和状态序号 |
| `board_painter.hpp/.cpp` | 棋盘、棋子、菜单、状态窗口和中文界面绘制 |
| `move_animation.hpp/.cpp` | 棋子移动插值动画及吃子过程显示 |
| `network_protocol.hpp/.cpp` | 联机消息结构、序列化和解析 |
| `network_transport.hpp/.cpp` | Winsock TCP 连接、后台收发和消息分帧 |
| `network_game_session.hpp/.cpp` | 主机权威同步、回合控制、重开和悔棋协商 |
| `assets/fonts/` | 简体中文字体及其许可证 |
| `third_party/raylib/` | raylib 头文件、导入库、运行时 DLL 和许可证 |
| `package_release.ps1/.bat` | Release 构建和发布包生成脚本 |

## 网络架构

```text
红方主机（权威棋局）
        │
        │ TCP / 27015
        │
黑方客户端（提交走法请求）
```

客户端不会直接决定走法结果。主机验证回合、状态序号和走法合法性后，向双方同步完整棋局状态。悔棋同样由主机保存历史快照并执行最终回滚，以防两端状态不一致。

## 第三方组件与许可证

- [raylib](https://www.raylib.com/)：许可证见 `third_party/raylib/LICENSE`
- [Noto CJK](https://github.com/notofonts/noto-cjk)：许可证见 `assets/fonts/LICENSE.txt`

本项目代码采用 [MIT License](LICENSE) 开源。
第三方组件分别遵循其原许可证。
