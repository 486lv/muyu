#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cctype>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct GameState {
    long long merit = 0;
    long long totalTaps = 0;
    int level = 1;
    int meritPerTap = 1;
    int combo = 0;
    int bestCombo = 0;
};

const std::vector<std::string> kBlessings = {
    "烦恼-1，功德+1。",
    "今天也要好好吃饭。",
    "心若平静，敲木鱼也有回响。",
    "好运正在路上，先敲为敬。",
    "功德圆满，从下一声开始。",
    "摸鱼不可取，敲木鱼可以。"
};

const std::vector<std::pair<long long, std::string>> kAchievements = {
    {10, "初入佛门"},
    {50, "小有功德"},
    {100, "木鱼熟练工"},
    {300, "功德批发商"},
    {666, "六六大顺"},
    {1000, "赛博高僧"}
};

std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

std::string savePath() {
    return "muyu_save.txt";
}

int levelForMerit(long long merit) {
    return static_cast<int>(merit / 100) + 1;
}

std::string randomBlessing() {
    std::uniform_int_distribution<std::size_t> dist(0, kBlessings.size() - 1);
    return kBlessings[dist(rng())];
}

void printBanner() {
    std::cout << R"BANNER(
  __  __ _   ___   ___   ___
 |  \/  | | | \ \ / / | | | |
 | |\/| | | | |\ V /| | | | |
 | |  | | |_| | | | | |_| | |
 |_|  |_|\___/  |_|  \___/|_|
        电子木鱼 / Cyber Wooden Fish

)BANNER";
}

void printHelp() {
    std::cout << "命令：\n"
              << "  直接回车        敲一次木鱼\n"
              << "  n <次数>        连续敲木鱼，例如 n 10\n"
              << "  stats           查看功德统计\n"
              << "  save            保存进度\n"
              << "  reset           重置当前进度\n"
              << "  help            查看帮助\n"
              << "  quit / exit     保存并退出\n\n";
}

void printState(const GameState& state) {
    std::cout << "功德：" << state.merit
              << " | 等级：" << state.level
              << " | 每次功德：" << state.meritPerTap
              << " | 总敲击：" << state.totalTaps
              << " | 连击：" << state.combo
              << " | 最佳连击：" << state.bestCombo << "\n";
}

void updateDerivedStats(GameState& state) {
    state.level = levelForMerit(state.merit);
    state.meritPerTap = std::max(1, state.level);
    state.bestCombo = std::max(state.bestCombo, state.combo);
}

bool isAchievementUnlocked(long long before, long long after, long long threshold) {
    return before < threshold && after >= threshold;
}

void tap(GameState& state, int times = 1) {
    if (times <= 0) {
        std::cout << "次数要大于 0。\n";
        return;
    }

    const long long before = state.merit;
    for (int i = 0; i < times; ++i) {
        state.merit += state.meritPerTap;
        state.totalTaps += 1;
        state.combo += 1;
        updateDerivedStats(state);
    }

    std::cout << "咚！x" << times << "  +" << (state.merit - before) << " 功德\n";
    std::cout << randomBlessing() << "\n";

    for (const auto& [threshold, title] : kAchievements) {
        if (isAchievementUnlocked(before, state.merit, threshold)) {
            std::cout << "解锁成就：[" << title << "]，累计功德达到 " << threshold << "。\n";
        }
    }

    printState(state);
}

bool saveGame(const GameState& state) {
    std::ofstream out(savePath());
    if (!out) {
        return false;
    }

    out << "merit=" << state.merit << "\n"
        << "totalTaps=" << state.totalTaps << "\n"
        << "bestCombo=" << state.bestCombo << "\n";
    return true;
}

bool loadGame(GameState& state) {
    std::ifstream in(savePath());
    if (!in) {
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        const auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        const std::string key = line.substr(0, pos);
        const std::string value = line.substr(pos + 1);
        try {
            if (key == "merit") {
                state.merit = std::stoll(value);
            } else if (key == "totalTaps") {
                state.totalTaps = std::stoll(value);
            } else if (key == "bestCombo") {
                state.bestCombo = std::stoi(value);
            }
        } catch (const std::exception&) {
            return false;
        }
    }

    updateDerivedStats(state);
    return true;
}

void resetGame(GameState& state) {
    state = GameState{};
    std::error_code ec;
    fs::remove(savePath(), ec);
    std::cout << "进度已重置。\n";
    printState(state);
}

std::string trim(const std::string& input) {
    const auto start = input.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = input.find_last_not_of(" \t\r\n");
    return input.substr(start, end - start + 1);
}

std::string toLower(std::string input) {
    std::transform(input.begin(), input.end(), input.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return input;
}

int main() {
    GameState state;
    printBanner();

    if (loadGame(state)) {
        std::cout << "已读取存档：" << savePath() << "\n";
    } else {
        std::cout << "未找到存档，从 0 功德开始。\n";
    }

    printHelp();
    printState(state);

    std::string line;
    while (true) {
        std::cout << "\n木鱼> ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\n输入结束，正在保存...\n";
            saveGame(state);
            break;
        }

        const std::string command = toLower(trim(line));
        if (command.empty()) {
            tap(state);
            continue;
        }

        if (command == "quit" || command == "exit" || command == "q") {
            if (saveGame(state)) {
                std::cout << "已保存进度，下次继续积攒功德。\n";
            } else {
                std::cout << "保存失败，请检查当前目录是否可写。\n";
            }
            break;
        }

        if (command == "help" || command == "h") {
            printHelp();
            continue;
        }

        if (command == "stats") {
            printState(state);
            continue;
        }

        if (command == "save") {
            std::cout << (saveGame(state) ? "保存成功。\n" : "保存失败。\n");
            continue;
        }

        if (command == "reset") {
            resetGame(state);
            continue;
        }

        std::istringstream iss(command);
        std::string op;
        int times = 0;
        iss >> op >> times;
        if ((op == "n" || op == "tap") && times > 0) {
            tap(state, times);
            continue;
        }

        std::cout << "未知命令：" << line << "。输入 help 查看命令。\n";
    }

    return 0;
}
