# 木鱼 Muyu

一个用 C++17 写的命令行电子木鱼小游戏。敲一下，功德 +1；敲多了会升级、解锁成就，并自动保存进度。

## 功能

- 直接回车敲木鱼，支持 `n 10` 连敲
- 功德、等级、每次功德、总敲击、最佳连击统计
- 随机祝福语和阶段成就
- 自动读取 / 保存本地存档 `muyu_save.txt`
- 支持 CMake，Visual Studio / g++ / clang++ 都可以编译

## 快速开始

### 使用 CMake

```bash
cmake -S . -B build
cmake --build build
./build/muyu
```

Windows 下可执行文件通常在：

```powershell
.\build\Debug\muyu.exe
```

### 使用 g++ 直接编译

```bash
g++ -std=c++17 src/main.cpp -o muyu
./muyu
```

## 游戏命令

| 命令 | 作用 |
| --- | --- |
| 直接回车 | 敲一次木鱼 |
| `n <次数>` | 连续敲木鱼，例如 `n 10` |
| `stats` | 查看当前统计 |
| `save` | 手动保存进度 |
| `reset` | 重置当前进度 |
| `help` | 查看帮助 |
| `quit` / `exit` | 保存并退出 |

## 项目结构

```text
muyu/
├── CMakeLists.txt
├── README.md
├── src/
│   └── main.cpp
└── .gitignore
```

## 后续可以继续加的功能

- 音效：敲击时播放“咚”声
- 图形界面：用 Qt / SFML 做一个真正的木鱼按钮
- 排行榜：记录每日功德和历史最高分
- 道具系统：例如双倍功德、自动敲木鱼
