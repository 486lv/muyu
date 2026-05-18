#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace muyu {

using i64 = long long;

struct Realm {
    i64 requiredMerit;
    std::string title;
    std::string description;
};

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    i64 requiredValue;
};

struct ShopItem {
    std::string id;
    std::string name;
    std::string description;
    i64 baseCost;
    int maxLevel;
};

struct Quest {
    std::string id;
    std::string title;
    std::string description;
    std::string metric;
    i64 target;
    i64 rewardMerit;
    i64 rewardFaith;
    i64 rewardJade;
    bool claimed = false;
};

struct GameState {
    i64 merit = 0;
    i64 lifetimeMerit = 0;
    i64 totalTaps = 0;
    i64 totalMeditations = 0;
    i64 totalPilgrimages = 0;
    i64 jade = 0;
    i64 faith = 0;
    int focus = 10;
    int maxFocus = 10;
    int combo = 0;
    int bestCombo = 0;
    int day = 1;
    int eventCountdown = 12;
    std::string sect = "云游散修";
    std::string woodenFishName = "旧木鱼";
    std::map<std::string, int> upgrades;
    std::map<std::string, bool> achievements;
    std::map<std::string, bool> questClaims;
};

std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

i64 randomInt(i64 minValue, i64 maxValue) {
    std::uniform_int_distribution<i64> dist(minValue, maxValue);
    return dist(rng());
}

const std::vector<Realm>& realms() {
    static const std::vector<Realm> data = {
        {0, "凡心未净", "刚刚拿起木鱼，心里还有奶茶和作业。"},
        {100, "初闻钟声", "开始听见咚声背后的节奏。"},
        {500, "寺门行者", "能稳定修行，也能稳定摸鱼。"},
        {1500, "禅院执事", "负责敲木鱼、管香火、顺便维护存档。"},
        {5000, "功德长老", "一声木鱼能让 bug 自己忏悔。"},
        {12000, "云端罗汉", "把木鱼敲到了云服务器上。"},
        {30000, "赛博菩萨", "功德与代码共同编译通过。"},
        {100000, "万法归咚", "咚声响起，世界安静。"}
    };
    return data;
}

const std::vector<std::string>& blessings() {
    static const std::vector<std::string> data = {
        "烦恼-1，功德+1。",
        "今日宜早睡，忌硬刚编译器。",
        "你敲下的不是木鱼，是人生的断点。",
        "一声咚，万念空；两声咚，作业也空。",
        "香火未必旺，心态一定要稳。",
        "木鱼提醒你：先保存，再重构。",
        "云深不知处，功德正在增长。",
        "这一声很轻，但它算数。"
    };
    return data;
}

const std::vector<Achievement>& achievements() {
    static const std::vector<Achievement> data = {
        {"tap_10", "初试木鱼", "累计敲击 10 次。", 10},
        {"tap_100", "手速渐稳", "累计敲击 100 次。", 100},
        {"tap_1000", "键盘罗汉", "累计敲击 1000 次。", 1000},
        {"merit_1000", "千功德", "累计功德达到 1000。", 1000},
        {"merit_10000", "万功德", "累计功德达到 10000。", 10000},
        {"combo_50", "五十连咚", "最佳连击达到 50。", 50},
        {"combo_200", "二百连咚", "最佳连击达到 200。", 200},
        {"jade_50", "香火小亨", "持有灵玉达到 50。", 50},
        {"faith_500", "信众云集", "信众达到 500。", 500},
        {"pilgrim_10", "行脚十方", "云游次数达到 10。", 10}
    };
    return data;
}

const std::vector<ShopItem>& shopItems() {
    static const std::vector<ShopItem> data = {
        {"fish", "升级木鱼", "提高每次敲击获得的基础功德。", 80, 20},
        {"incense", "添置香炉", "提高香火与随机事件收益。", 120, 15},
        {"mat", "蒲团静室", "提高专注上限与打坐收益。", 150, 12},
        {"bell", "晨钟暮鼓", "提高连击带来的功德倍率。", 220, 10},
        {"scripture", "藏经阁", "提高任务与云游奖励。", 300, 10}
    };
    return data;
}

std::vector<Quest> buildQuests(const GameState& state) {
    const int chapter = std::max(1, state.day / 3 + 1);
    std::vector<Quest> q = {
        {"daily_tap", "晨课木鱼", "完成今日晨课敲击。", "taps", 20LL * chapter, 40LL * chapter, 8LL * chapter, 1LL * chapter},
        {"daily_merit", "积攒功德", "让寺院香火稳定增长。", "merit", 120LL * chapter, 80LL * chapter, 12LL * chapter, 2LL * chapter},
        {"daily_focus", "静坐凝神", "通过打坐恢复内心秩序。", "meditation", std::max<i64>(1, chapter), 60LL * chapter, 20LL * chapter, 1LL * chapter}
    };
    for (auto& quest : q) {
        const std::string key = quest.id + "_day_" + std::to_string(state.day);
        quest.claimed = state.questClaims.count(key) > 0;
    }
    return q;
}

std::string savePath() {
    return "muyu_save.txt";
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

std::vector<std::string> split(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> parts;
    std::string token;
    while (iss >> token) {
        parts.push_back(token);
    }
    return parts;
}

int upgradeLevel(const GameState& state, const std::string& id) {
    const auto it = state.upgrades.find(id);
    return it == state.upgrades.end() ? 0 : it->second;
}

const Realm& currentRealm(const GameState& state) {
    const auto& all = realms();
    const Realm* result = &all.front();
    for (const auto& realm : all) {
        if (state.lifetimeMerit >= realm.requiredMerit) {
            result = &realm;
        }
    }
    return *result;
}

const Realm* nextRealm(const GameState& state) {
    for (const auto& realm : realms()) {
        if (state.lifetimeMerit < realm.requiredMerit) {
            return &realm;
        }
    }
    return nullptr;
}

i64 baseMeritPerTap(const GameState& state) {
    const int fish = upgradeLevel(state, "fish");
    const int scripture = upgradeLevel(state, "scripture");
    return 1 + fish * 2 + scripture;
}

double comboMultiplier(const GameState& state) {
    const int bell = upgradeLevel(state, "bell");
    const double comboBonus = std::min(2.5, state.combo / 75.0);
    return 1.0 + comboBonus + bell * 0.04;
}

i64 calculateTapReward(const GameState& state) {
    const double value = baseMeritPerTap(state) * comboMultiplier(state);
    return std::max<i64>(1, static_cast<i64>(value));
}

void normalize(GameState& state) {
    state.maxFocus = 10 + upgradeLevel(state, "mat") * 3;
    state.focus = std::clamp(state.focus, 0, state.maxFocus);
    state.bestCombo = std::max(state.bestCombo, state.combo);
    if (state.eventCountdown <= 0) {
        state.eventCountdown = 1;
    }
}

void printBanner() {
    std::cout << R"BANNER(
 __  __ _   ___   ___   ___      ____  _                 _       _
|  \/  | | | \ \ / / | | | |    / ___|(_)_ __ ___  _   _| | __ _| |_ ___  _ __
| |\/| | | | |\ V /| | | | |    \___ \| | '_ ` _ \| | | | |/ _` | __/ _ \| '__|
| |  | | |_| | | | | |_| | |     ___) | | | | | | | |_| | | (_| | || (_) | |
|_|  |_|\___/  |_|  \___/|_|    |____/|_|_| |_| |_|\__,_|_|\__,_|\__\___/|_|

                木鱼：万法归咚 / Grand Muyu Simulator
)BANNER";
}

void printHelp() {
    std::cout << "\n核心命令\n"
              << "  enter              敲一次木鱼\n"
              << "  n <次数>           连续敲木鱼，例如 n 50\n"
              << "  chant              诵经，消耗专注换取功德和信众\n"
              << "  meditate           打坐，恢复专注并获得少量信众\n"
              << "  pilgrimage         云游，有风险的高收益探索\n"
              << "\n经营命令\n"
              << "  status             查看总览\n"
              << "  shop               查看寺院建设商店\n"
              << "  buy <编号/id>      购买升级，例如 buy 1 或 buy fish\n"
              << "  quests             查看今日任务\n"
              << "  claim              领取已完成任务奖励\n"
              << "  achievements       查看成就\n"
              << "  rest               进入下一天，刷新任务与事件节奏\n"
              << "\n系统命令\n"
              << "  save               保存进度\n"
              << "  reset              重置存档\n"
              << "  help               查看帮助\n"
              << "  quit / exit        保存并退出\n";
}

void printStatus(const GameState& state) {
    const Realm& realm = currentRealm(state);
    std::cout << "\n=== 寺院总览 ===\n";
    std::cout << "门派：" << state.sect << " | 境界：" << realm.title << "\n";
    std::cout << "说明：" << realm.description << "\n";
    std::cout << "当前功德：" << state.merit
              << " | 累计功德：" << state.lifetimeMerit
              << " | 灵玉：" << state.jade
              << " | 信众：" << state.faith << "\n";
    std::cout << "专注：" << state.focus << "/" << state.maxFocus
              << " | 今日：第 " << state.day << " 天"
              << " | 总敲击：" << state.totalTaps << "\n";
    std::cout << "连击：" << state.combo
              << " | 最佳连击：" << state.bestCombo
              << " | 当前每敲约 +" << calculateTapReward(state) << " 功德\n";

    if (const Realm* next = nextRealm(state)) {
        std::cout << "下一境界：" << next->title << "，还需 "
                  << std::max<i64>(0, next->requiredMerit - state.lifetimeMerit) << " 累计功德\n";
    } else {
        std::cout << "境界已达顶峰：万法归咚。\n";
    }
}

void printShop(const GameState& state) {
    std::cout << "\n=== 寺院建设 ===\n";
    const auto& items = shopItems();
    for (std::size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        const int level = upgradeLevel(state, item.id);
        const i64 cost = item.baseCost * (level + 1) * (level + 1);
        std::cout << std::setw(2) << (i + 1) << ". " << item.name
                  << " [" << item.id << "] Lv." << level << "/" << item.maxLevel;
        if (level >= item.maxLevel) {
            std::cout << " 已满级\n";
        } else {
            std::cout << " 价格：" << cost << " 功德\n";
        }
        std::cout << "    " << item.description << "\n";
    }
}

void printAchievements(const GameState& state) {
    std::cout << "\n=== 成就碑 ===\n";
    int unlocked = 0;
    for (const auto& achievement : achievements()) {
        const bool done = state.achievements.count(achievement.id) > 0;
        unlocked += done ? 1 : 0;
        std::cout << (done ? "[已解锁] " : "[未解锁] ")
                  << achievement.name << " - " << achievement.description << "\n";
    }
    std::cout << "进度：" << unlocked << "/" << achievements().size() << "\n";
}

i64 metricValue(const GameState& state, const Quest& quest) {
    if (quest.metric == "taps") {
        return state.totalTaps;
    }
    if (quest.metric == "merit") {
        return state.lifetimeMerit;
    }
    if (quest.metric == "meditation") {
        return state.totalMeditations;
    }
    return 0;
}

void printQuests(const GameState& state) {
    std::cout << "\n=== 今日功课，第 " << state.day << " 天 ===\n";
    auto questList = buildQuests(state);
    for (const auto& quest : questList) {
        const i64 current = metricValue(state, quest);
        const bool complete = current >= quest.target;
        std::cout << (quest.claimed ? "[已领取] " : (complete ? "[可领取] " : "[进行中] "))
                  << quest.title << " - " << quest.description << "\n"
                  << "    进度：" << std::min(current, quest.target) << "/" << quest.target
                  << " | 奖励：" << quest.rewardMerit << "功德, "
                  << quest.rewardFaith << "信众, " << quest.rewardJade << "灵玉\n";
    }
}

void unlockAchievements(GameState& state) {
    const std::map<std::string, i64> values = {
        {"tap_10", state.totalTaps},
        {"tap_100", state.totalTaps},
        {"tap_1000", state.totalTaps},
        {"merit_1000", state.lifetimeMerit},
        {"merit_10000", state.lifetimeMerit},
        {"combo_50", state.bestCombo},
        {"combo_200", state.bestCombo},
        {"jade_50", state.jade},
        {"faith_500", state.faith},
        {"pilgrim_10", state.totalPilgrimages}
    };

    for (const auto& achievement : achievements()) {
        if (state.achievements.count(achievement.id) > 0) {
            continue;
        }
        const auto it = values.find(achievement.id);
        if (it != values.end() && it->second >= achievement.requiredValue) {
            state.achievements[achievement.id] = true;
            state.jade += 2;
            std::cout << "\n成就解锁：【" << achievement.name << "】" << achievement.description
                      << " 奖励灵玉 +2\n";
        }
    }
}

std::string randomBlessing() {
    const auto& all = blessings();
    return all[static_cast<std::size_t>(randomInt(0, static_cast<i64>(all.size() - 1)))];
}

void randomEvent(GameState& state) {
    std::cout << "\n--- 随机事件 ---\n";
    const int incense = upgradeLevel(state, "incense");
    const int scripture = upgradeLevel(state, "scripture");
    const int roll = static_cast<int>(randomInt(1, 100));

    if (roll <= 25) {
        const i64 gain = 30 + incense * 15 + randomInt(0, 40);
        state.faith += gain;
        std::cout << "山下香客听见木鱼声，结伴上山。信众 +" << gain << "\n";
    } else if (roll <= 45) {
        const i64 gain = 2 + incense + randomInt(0, 3);
        state.jade += gain;
        std::cout << "香炉中凝出灵玉。灵玉 +" << gain << "\n";
    } else if (roll <= 65) {
        const i64 gain = 80 + scripture * 30 + randomInt(0, 120);
        state.merit += gain;
        state.lifetimeMerit += gain;
        std::cout << "你在藏经阁翻到残卷，心有所悟。功德 +" << gain << "\n";
    } else if (roll <= 82) {
        const int lost = std::min(state.focus, static_cast<int>(randomInt(1, 4)));
        state.focus -= lost;
        state.combo = 0;
        std::cout << "忽起杂念，连击中断，专注 -" << lost << "\n";
    } else {
        const i64 gain = 150 + state.day * 10 + randomInt(0, 100);
        state.merit += gain;
        state.lifetimeMerit += gain;
        state.faith += 20 + incense * 5;
        std::cout << "寺中钟鼓自鸣，众人称奇。功德 +" << gain << "，信众增加。\n";
    }
    state.eventCountdown = static_cast<int>(randomInt(9, 18));
    normalize(state);
    unlockAchievements(state);
}

void tap(GameState& state, int times) {
    times = std::clamp(times, 1, 100000);
    i64 gained = 0;
    for (int i = 0; i < times; ++i) {
        state.combo += 1;
        const i64 reward = calculateTapReward(state);
        gained += reward;
        state.merit += reward;
        state.lifetimeMerit += reward;
        state.totalTaps += 1;
        state.bestCombo = std::max<i64>(state.bestCombo, state.combo);

        state.eventCountdown -= 1;
        if (state.eventCountdown <= 0) {
            break;
        }
    }

    std::cout << "\n咚！x" << times << "，功德 +" << gained << "\n";
    std::cout << randomBlessing() << "\n";
    if (state.eventCountdown <= 0) {
        randomEvent(state);
    }
    normalize(state);
    unlockAchievements(state);
}

void chant(GameState& state) {
    if (state.focus < 3) {
        std::cout << "专注不足，先 meditate 打坐恢复。\n";
        return;
    }
    const int scripture = upgradeLevel(state, "scripture");
    const i64 gain = 50 + scripture * 35 + state.faith / 20;
    state.focus -= 3;
    state.combo = 0;
    state.merit += gain;
    state.lifetimeMerit += gain;
    state.faith += 5 + scripture;
    std::cout << "你诵读经文，心绪渐明。功德 +" << gain << "，信众 +" << (5 + scripture) << "\n";
    unlockAchievements(state);
}

void meditate(GameState& state) {
    const int mat = upgradeLevel(state, "mat");
    const int recover = 4 + mat * 2;
    const i64 faithGain = 2 + mat;
    state.totalMeditations += 1;
    state.focus = std::min(state.maxFocus, state.focus + recover);
    state.faith += faithGain;
    state.combo = 0;
    std::cout << "你在蒲团上打坐。专注恢复 +" << recover << "，信众 +" << faithGain << "\n";
    unlockAchievements(state);
}

void pilgrimage(GameState& state) {
    if (state.focus < 5) {
        std::cout << "云游至少需要 5 点专注。\n";
        return;
    }
    state.focus -= 5;
    state.combo = 0;
    state.totalPilgrimages += 1;

    const int scripture = upgradeLevel(state, "scripture");
    const int incense = upgradeLevel(state, "incense");
    const int roll = static_cast<int>(randomInt(1, 100));
    if (roll <= 20) {
        const i64 loss = std::min(state.merit, 50 + randomInt(0, 80));
        state.merit -= loss;
        std::cout << "云游遇雨，香火受损。当前功德 -" << loss << "\n";
    } else if (roll <= 65) {
        const i64 gain = 160 + scripture * 60 + randomInt(0, 180);
        const i64 faithGain = 20 + incense * 8;
        state.merit += gain;
        state.lifetimeMerit += gain;
        state.faith += faithGain;
        std::cout << "你为山村讲经，众人受益。功德 +" << gain << "，信众 +" << faithGain << "\n";
    } else {
        const i64 meritGain = 300 + scripture * 90 + randomInt(0, 300);
        const i64 jadeGain = 5 + randomInt(0, 6);
        state.merit += meritGain;
        state.lifetimeMerit += meritGain;
        state.jade += jadeGain;
        std::cout << "你发现古寺遗迹，得无名偈语。功德 +" << meritGain << "，灵玉 +" << jadeGain << "\n";
    }
    unlockAchievements(state);
}

void buy(GameState& state, const std::string& selector) {
    const auto& items = shopItems();
    const ShopItem* chosen = nullptr;

    if (!selector.empty() && std::all_of(selector.begin(), selector.end(), [](unsigned char ch) { return std::isdigit(ch); })) {
        const int index = std::stoi(selector);
        if (index >= 1 && index <= static_cast<int>(items.size())) {
            chosen = &items[static_cast<std::size_t>(index - 1)];
        }
    } else {
        for (const auto& item : items) {
            if (item.id == selector) {
                chosen = &item;
                break;
            }
        }
    }

    if (chosen == nullptr) {
        std::cout << "没有找到这个升级。输入 shop 查看列表。\n";
        return;
    }

    int level = upgradeLevel(state, chosen->id);
    if (level >= chosen->maxLevel) {
        std::cout << chosen->name << " 已经满级。\n";
        return;
    }

    const i64 cost = chosen->baseCost * (level + 1) * (level + 1);
    if (state.merit < cost) {
        std::cout << "功德不足，需要 " << cost << "，当前只有 " << state.merit << "。\n";
        return;
    }

    state.merit -= cost;
    state.upgrades[chosen->id] = level + 1;
    normalize(state);
    std::cout << "升级成功：" << chosen->name << " Lv." << (level + 1) << "\n";
}

void claimQuests(GameState& state) {
    auto questList = buildQuests(state);
    bool any = false;
    const int scripture = upgradeLevel(state, "scripture");
    for (const auto& quest : questList) {
        const std::string key = quest.id + "_day_" + std::to_string(state.day);
        if (state.questClaims.count(key) > 0) {
            continue;
        }
        if (metricValue(state, quest) < quest.target) {
            continue;
        }
        any = true;
        const i64 merit = quest.rewardMerit + scripture * 10;
        const i64 faith = quest.rewardFaith + scripture * 2;
        const i64 jade = quest.rewardJade;
        state.merit += merit;
        state.lifetimeMerit += merit;
        state.faith += faith;
        state.jade += jade;
        state.questClaims[key] = true;
        std::cout << "领取任务【" << quest.title << "】：功德 +" << merit
                  << "，信众 +" << faith << "，灵玉 +" << jade << "\n";
    }
    if (!any) {
        std::cout << "暂无可领取任务。\n";
    }
    unlockAchievements(state);
}

void rest(GameState& state) {
    state.day += 1;
    state.combo = 0;
    state.focus = state.maxFocus;
    state.eventCountdown = static_cast<int>(randomInt(10, 18));
    const i64 passiveFaith = 3 + upgradeLevel(state, "incense") * 2;
    state.faith += passiveFaith;
    std::cout << "\n夜色归山。第 " << state.day << " 天开始。专注已恢复，信众 +" << passiveFaith << "\n";
}

bool saveGame(const GameState& state) {
    std::ofstream out(savePath());
    if (!out) {
        return false;
    }
    out << "version=2\n";
    out << "merit=" << state.merit << "\n";
    out << "lifetimeMerit=" << state.lifetimeMerit << "\n";
    out << "totalTaps=" << state.totalTaps << "\n";
    out << "totalMeditations=" << state.totalMeditations << "\n";
    out << "totalPilgrimages=" << state.totalPilgrimages << "\n";
    out << "jade=" << state.jade << "\n";
    out << "faith=" << state.faith << "\n";
    out << "focus=" << state.focus << "\n";
    out << "maxFocus=" << state.maxFocus << "\n";
    out << "combo=" << state.combo << "\n";
    out << "bestCombo=" << state.bestCombo << "\n";
    out << "day=" << state.day << "\n";
    out << "eventCountdown=" << state.eventCountdown << "\n";
    out << "sect=" << state.sect << "\n";
    out << "woodenFishName=" << state.woodenFishName << "\n";
    for (const auto& [id, level] : state.upgrades) {
        out << "upgrade." << id << "=" << level << "\n";
    }
    for (const auto& [id, done] : state.achievements) {
        if (done) {
            out << "achievement." << id << "=1\n";
        }
    }
    for (const auto& [id, done] : state.questClaims) {
        if (done) {
            out << "quest." << id << "=1\n";
        }
    }
    return true;
}

void setNumber(i64& target, const std::string& value) {
    target = std::stoll(value);
}

void setNumber(int& target, const std::string& value) {
    target = std::stoi(value);
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
            if (key == "merit") setNumber(state.merit, value);
            else if (key == "lifetimeMerit") setNumber(state.lifetimeMerit, value);
            else if (key == "totalTaps") setNumber(state.totalTaps, value);
            else if (key == "totalMeditations") setNumber(state.totalMeditations, value);
            else if (key == "totalPilgrimages") setNumber(state.totalPilgrimages, value);
            else if (key == "jade") setNumber(state.jade, value);
            else if (key == "faith") setNumber(state.faith, value);
            else if (key == "focus") setNumber(state.focus, value);
            else if (key == "maxFocus") setNumber(state.maxFocus, value);
            else if (key == "combo") setNumber(state.combo, value);
            else if (key == "bestCombo") setNumber(state.bestCombo, value);
            else if (key == "day") setNumber(state.day, value);
            else if (key == "eventCountdown") setNumber(state.eventCountdown, value);
            else if (key == "sect") state.sect = value;
            else if (key == "woodenFishName") state.woodenFishName = value;
            else if (key.rfind("upgrade.", 0) == 0) state.upgrades[key.substr(8)] = std::stoi(value);
            else if (key.rfind("achievement.", 0) == 0) state.achievements[key.substr(12)] = value == "1";
            else if (key.rfind("quest.", 0) == 0) state.questClaims[key.substr(6)] = value == "1";
        } catch (const std::exception&) {
            std::cout << "存档字段损坏，已跳过：" << key << "\n";
        }
    }

    if (state.lifetimeMerit < state.merit) {
        state.lifetimeMerit = state.merit;
    }
    normalize(state);
    return true;
}

void resetGame(GameState& state) {
    state = GameState{};
    std::error_code ec;
    fs::remove(savePath(), ec);
    std::cout << "存档已重置。\n";
}

void chooseSect(GameState& state) {
    std::cout << "选择你的修行路线：\n"
              << "  1. 云隐寺：平衡发展\n"
              << "  2. 赛博禅院：敲击成长更快\n"
              << "  3. 香火道场：信众与灵玉更多\n"
              << "输入编号，或直接回车选择云隐寺：";
    std::string line;
    std::getline(std::cin, line);
    line = trim(line);
    if (line == "2") {
        state.sect = "赛博禅院";
        state.upgrades["fish"] = 1;
    } else if (line == "3") {
        state.sect = "香火道场";
        state.upgrades["incense"] = 1;
        state.jade = 3;
    } else {
        state.sect = "云隐寺";
        state.focus = 12;
        state.maxFocus = 12;
    }
    normalize(state);
    std::cout << "你加入了【" << state.sect << "】。\n";
}

void handleCommand(GameState& state, const std::string& raw, bool& running) {
    const std::string commandLine = toLower(trim(raw));
    if (commandLine.empty()) {
        tap(state, 1);
        return;
    }

    const auto parts = split(commandLine);
    const std::string& cmd = parts.front();

    if (cmd == "quit" || cmd == "exit" || cmd == "q") {
        if (saveGame(state)) {
            std::cout << "已保存。愿你下次继续万法归咚。\n";
        } else {
            std::cout << "保存失败，请检查当前目录权限。\n";
        }
        running = false;
    } else if (cmd == "help" || cmd == "h") {
        printHelp();
    } else if (cmd == "status" || cmd == "stats") {
        printStatus(state);
    } else if (cmd == "shop") {
        printShop(state);
    } else if (cmd == "buy") {
        if (parts.size() < 2) {
            std::cout << "用法：buy <编号/id>\n";
        } else {
            buy(state, parts[1]);
        }
    } else if (cmd == "quests" || cmd == "quest") {
        printQuests(state);
    } else if (cmd == "claim") {
        claimQuests(state);
    } else if (cmd == "achievements" || cmd == "ach") {
        printAchievements(state);
    } else if (cmd == "chant") {
        chant(state);
    } else if (cmd == "meditate" || cmd == "zen") {
        meditate(state);
    } else if (cmd == "pilgrimage" || cmd == "travel") {
        pilgrimage(state);
    } else if (cmd == "rest" || cmd == "next") {
        rest(state);
    } else if (cmd == "save") {
        std::cout << (saveGame(state) ? "保存成功。\n" : "保存失败。\n");
    } else if (cmd == "reset") {
        std::cout << "确定重置？输入 YES：";
        std::string confirm;
        std::getline(std::cin, confirm);
        if (confirm == "YES") {
            resetGame(state);
        } else {
            std::cout << "已取消。\n";
        }
    } else if (cmd == "n" || cmd == "tap") {
        int times = 1;
        if (parts.size() >= 2) {
            try {
                times = std::stoi(parts[1]);
            } catch (const std::exception&) {
                std::cout << "次数格式不正确。\n";
                return;
            }
        }
        tap(state, times);
    } else {
        std::cout << "未知命令：" << raw << "。输入 help 查看命令。\n";
    }
}

}  // namespace muyu

int main() {
    using namespace muyu;

    GameState state;
    printBanner();

    if (loadGame(state)) {
        std::cout << "读取存档成功：" << savePath() << "\n";
    } else {
        std::cout << "没有发现存档。\n";
        chooseSect(state);
    }

    printStatus(state);
    printHelp();

    bool running = true;
    std::string line;
    while (running) {
        std::cout << "\n第" << state.day << "天 · " << currentRealm(state).title << " · 木鱼> ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\n输入结束，自动保存。\n";
            saveGame(state);
            break;
        }
        handleCommand(state, line, running);
        normalize(state);
    }

    return 0;
}
