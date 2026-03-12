#include <bits/stdc++.h>
using namespace std;

namespace Cfg {
    constexpr int kHpBarWidth = 20;
    constexpr int kEnemyBarWidth = 10;
    constexpr int kHerbHeal = 30;
    constexpr int kGateWrongDmg = 15;
    constexpr int kGateForceDmg = 20;
    constexpr int kCampHealMin = 15;
    constexpr int kCampHealMax = 25;
    constexpr int kTitlePad = 44;
    constexpr bool kUseAnsi = true;
    constexpr bool kDoClear = true;
}

namespace Color {
    const string RESET   = Cfg::kUseAnsi ? "\033[0m"  : "";
    const string GREEN   = Cfg::kUseAnsi ? "\033[92m" : "";
    const string YELLOW  = Cfg::kUseAnsi ? "\033[93m" : "";
    const string RED     = Cfg::kUseAnsi ? "\033[91m" : "";
    const string CYAN    = Cfg::kUseAnsi ? "\033[96m" : "";
    const string WHITE   = Cfg::kUseAnsi ? "\033[97m" : "";
    const string DIM     = Cfg::kUseAnsi ? "\033[2m"  : "";
    const string BOLD    = Cfg::kUseAnsi ? "\033[1m"  : "";
    const string MAGENTA = Cfg::kUseAnsi ? "\033[95m" : "";
}

mt19937 rng((uint32_t)chrono::steady_clock::now().time_since_epoch().count());
int randRange(int lo, int hi) { return uniform_int_distribution<int>(lo, hi)(rng); }

void clearScreen() {
    if (!Cfg::kDoClear) return;
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void sleep_ms(int ms) { this_thread::sleep_for(chrono::milliseconds(ms)); }
void flushLine() { cin.ignore(numeric_limits<streamsize>::max(), '\n'); }

void print(const string& text, const string& color = Color::WHITE, int delay_ms = 0) {
    if (delay_ms) sleep_ms(delay_ms);
    cout << color << text << Color::RESET << "\n";
}

void printHeader(const string& title) {
    cout << "\n" << Color::GREEN
         << "╔══════════════════════════════════════════════╗\n"
         << "║  " << Color::BOLD << title;
    int pad = max(0, Cfg::kTitlePad - (int)title.size());
    for (int i = 0; i < pad; i++) cout << " ";
    cout << Color::GREEN << "║\n"
         << "╚══════════════════════════════════════════════╝\n"
         << Color::RESET;
}

void printDivider() { cout << Color::DIM << "──────────────────────────────────────────────\n" << Color::RESET; }

struct Player {
    string name;
    int hp = 100, maxHp = 100, atk = 10, gold = 0, floor = 1, kills = 0;
    vector<string> inventory;

    bool has(const string& item) const { return find(inventory.begin(), inventory.end(), item) != inventory.end(); }
    void add(const string& item) { inventory.push_back(item); print("  [+] Added: " + item, Color::YELLOW); }
    void remove(const string& item) {
        auto it = find(inventory.begin(), inventory.end(), item);
        if (it != inventory.end()) inventory.erase(it);
    }

    void show() const {
        printDivider();
        cout << Color::GREEN << "  " << Color::BOLD << name << Color::RESET << "\n";
        int bars = (hp * Cfg::kHpBarWidth) / maxHp;
        cout << "  HP  [";
        for (int i = 0; i < Cfg::kHpBarWidth; i++)
            cout << (i < bars ? (hp > 50 ? Color::GREEN : hp > 25 ? Color::YELLOW : Color::RED) + string("█")
                               : Color::DIM + string("░"));
        cout << Color::RESET << "] " << hp << "/" << maxHp << "\n";
        cout << "  ATK " << Color::CYAN << atk << Color::RESET
             << "  |  GOLD " << Color::YELLOW << gold << Color::RESET
             << "  |  FLOOR " << Color::MAGENTA << floor << Color::RESET
             << "  |  KILLS " << Color::RED << kills << Color::RESET << "\n";
        if (!inventory.empty()) {
            cout << "  BAG ";
            for (auto& i : inventory) cout << "[" << Color::YELLOW << i << Color::RESET << "] ";
            cout << "\n";
        }
        printDivider();
    }
};

struct Enemy {
    string name; int hp, maxHp, atk, gold; string drop;
    Enemy(string n, int h, int a, int g, string d = "") : name(move(n)), hp(h), maxHp(h), atk(a), gold(g), drop(move(d)) {}
};

enum class Result { WIN, DEAD, FLED };

Result combat(Player& p, Enemy e) {
    print("\n  ⚠  " + e.name + " appears!", Color::RED);
    sleep_ms(200);

    while (true) {
        printDivider();
        int eBars = (e.hp * Cfg::kEnemyBarWidth) / e.maxHp;
        cout << "  " << Color::RED << e.name << " HP [";
        for (int i = 0; i < Cfg::kEnemyBarWidth; i++) cout << (i < eBars ? "█" : "░");
        cout << "] " << e.hp << Color::RESET << "\n\n";

        cout << "  [1] Attack\n"
             << "  [2] Use Healing Herb" << (p.has("Healing Herb") ? "" : Color::DIM + string(" (none)") + Color::RESET) << "\n"
             << "  [3] Flee (50%)\n\n"
             << Color::GREEN << "  > " << Color::RESET;

        int c; if (!(cin >> c)) return Result::DEAD; flushLine();

        if (c == 1) {
            int dmg = p.atk + randRange(0, 5);
            e.hp -= dmg;
            print("  You strike for " + to_string(dmg) + ".", Color::GREEN, 80);
            if (e.hp <= 0) {
                print("  " + e.name + " defeated!", Color::YELLOW, 150);
                p.gold += e.gold; p.kills++;
                if (!e.drop.empty()) { print("  Dropped: " + e.drop, Color::YELLOW); p.add(e.drop); }
                print("  +" + to_string(e.gold) + " gold", Color::YELLOW, 120);
                return Result::WIN;
            }
            int edm = e.atk + randRange(0, 4);
            p.hp = max(0, p.hp - edm);
            print("  " + e.name + " hits for " + to_string(edm) + ".", Color::RED, 150);
            p.show();
            if (p.hp <= 0) return Result::DEAD;
        }
        else if (c == 2) {
            if (!p.has("Healing Herb")) print("  No herbs.", Color::DIM);
            else { p.remove("Healing Herb"); p.hp = min(p.maxHp, p.hp + Cfg::kHerbHeal); print("  +" + to_string(Cfg::kHerbHeal) + " HP", Color::GREEN, 120); }
        }
        else if (c == 3) {
            if (randRange(0,1)) { print("  You escape!", Color::CYAN, 120); return Result::FLED; }
            int edm = e.atk + randRange(0, 4);
            p.hp = max(0, p.hp - edm);
            print("  Escape failed! -" + to_string(edm) + " HP", Color::RED, 120);
            p.show();
            if (p.hp <= 0) return Result::DEAD;
        }
        else print("  Invalid choice.", Color::DIM);
    }
}

bool floor1(Player& p); bool floor2(Player& p); bool floor3(Player& p);

bool floor1(Player& p) {
    p.floor = 1; clearScreen(); printHeader("FLOOR 1 — THE DUNGEON ENTRANCE");
    print("  Cold air rises from the depths. Your lantern quivers.", Color::WHITE);

    bool hasKey=false, visitedMarket=false, visitedCorridor=false;

    while (true) {
        printDivider();
        cout << "  [1] Left passage  — rushing water\n"
             << "  [2] Center passage — locked door\n"
             << "  [3] Right passage  — torchlight\n"
             << "  [4] Check stats\n"
             << "  [5] Descend to floor 2 (need key)\n\n"
             << Color::GREEN << "  > " << Color::RESET;
        int c; if (!(cin >> c)) return false; flushLine();

        if (c == 1) {
            clearScreen(); printHeader("THE UNDERGROUND MARKET");
            print("  A lone merchant peers up. \"Coin for wares?\"", Color::CYAN);
            while (true) {
                p.show();
                cout << "  [1] Healing Herb (15g)\n"
                     << "  [2] Iron Sword  (20g, +8 ATK)\n"
                     << "  [3] Leave\n\n"
                     << Color::GREEN << "  > " << Color::RESET;
                int mc; if (!(cin >> mc)) return false; flushLine();
                if (mc == 1) { if (p.gold < 15) print("  Too poor.", Color::RED); else { p.gold -= 15; p.add("Healing Herb"); } }
                else if (mc == 2) { if (p.has("Iron Sword")) print("  Already owned.", Color::DIM);
                                    else if (p.gold < 20) print("  Too poor.", Color::RED);
                                    else { p.gold -= 20; p.atk += 8; p.add("Iron Sword"); print("  ATK +8", Color::YELLOW); } }
                else break;
            }
            visitedMarket = true;
        }
        else if (c == 2) {
            clearScreen(); printHeader("THE LOCKED CHAMBER");
            if (!hasKey) print("  Thick iron door; you lack a key.", Color::RED);
            else {
                print("  You unlock the door. Treasure spills out!", Color::YELLOW);
                p.gold += 40; p.add("Healing Herb"); hasKey = false; p.remove("Old Key");
            }
        }
        else if (c == 3) {
            if (visitedCorridor) { print("  The corridor is quiet now.", Color::DIM); continue; }
            clearScreen(); printHeader("THE TORCH CORRIDOR");
            print("  A Skeleton Guard shambles forward.", Color::RED);
            Enemy skel("Skeleton Guard", 35, 7, 18, "Old Key");
            auto res = combat(p, skel);
            if (res == Result::DEAD) return false;
            if (res == Result::WIN) { hasKey = true; visitedCorridor = true; }
        }
        else if (c == 4) { clearScreen(); p.show(); }
        else if (c == 5) {
            if (!hasKey && !visitedCorridor) print("  The staircase is blocked; find a key.", Color::RED);
            else { print("  You descend...", Color::CYAN); sleep_ms(400); return floor2(p); }
        }
    }
}

bool floor2(Player& p) {
    p.floor = 2; clearScreen(); printHeader("FLOOR 2 — THE RIDDLED DEPTHS");
    bool riddleSolved=false, bossDead=false;

    while (true) {
        printDivider();
        cout << "  [1] Riddle Gate\n"
             << "  [2] Dark Alcove\n"
             << "  [3] Campfire\n"
             << "  [4] Check stats\n"
             << "  [5] Descend to floor 3" << (riddleSolved ? "" : Color::DIM + string(" (locked)") + Color::RESET) << "\n\n"
             << Color::GREEN << "  > " << Color::RESET;
        int c; if (!(cin >> c)) return false; flushLine();

        if (c == 1) {
            if (riddleSolved) { print("  The gate is already open.", Color::DIM); continue; }
            clearScreen(); printHeader("THE RIDDLE GATE");
            print("  \"I speak without a mouth and hear without ears.\"", Color::CYAN);
            print("  \"I have no body, but come alive with the wind.\"", Color::CYAN);
            print("  What am I?\n", Color::CYAN);
            cout << "  [1] An Echo\n  [2] The Wind\n  [3] A Shadow\n  [4] Force it open (-20 HP)\n\n"
                 << Color::GREEN << "  > " << Color::RESET;
            int rc; if (!(cin >> rc)) return false; flushLine();
            if (rc == 1) { print("  The gate rumbles open.", Color::GREEN); riddleSolved = true; }
            else if (rc == 4) { p.hp = max(0, p.hp - Cfg::kGateForceDmg); print("  Painful, but it opens.", Color::RED); p.show(); riddleSolved = true; if (p.hp <= 0) return false; }
            else { p.hp = max(0, p.hp - Cfg::kGateWrongDmg); print("  Spikes lash out! -" + to_string(Cfg::kGateWrongDmg) + " HP", Color::RED); p.show(); if (p.hp <= 0) return false; }
        }
        else if (c == 2) {
            if (bossDead) { print("  The alcove is empty.", Color::DIM); continue; }
            clearScreen(); printHeader("THE DARK ALCOVE");
            print("  A Giant Spider drops from above!", Color::RED);
            Enemy spider("Giant Spider", 55, 12, 30, "Venom Sac");
            auto res = combat(p, spider);
            if (res == Result::DEAD) return false;
            if (res == Result::WIN) bossDead = true;
        }
        else if (c == 3) {
            clearScreen(); printHeader("THE CAMPFIRE");
            int heal = randRange(Cfg::kCampHealMin, Cfg::kCampHealMax);
            p.hp = min(p.maxHp, p.hp + heal);
            print("  You rest. +" + to_string(heal) + " HP.", Color::GREEN); p.show();
        }
        else if (c == 4) { clearScreen(); p.show(); }
        else if (c == 5) {
            if (!riddleSolved) print("  The gate is shut tight.", Color::RED);
            else { print("  You step through the gate...", Color::CYAN); sleep_ms(400); return floor3(p); }
        }
    }
}

bool floor3(Player& p) {
    p.floor = 3; clearScreen(); printHeader("FLOOR 3 — THE VOID SANCTUM");
    print("  A shadowy figure coalesces — the VOID WRAITH.", Color::RED);
    if (p.has("Venom Sac")) { print("  You coat your blade with venom. ATK +5!", Color::GREEN); p.atk += 5; p.remove("Venom Sac"); }
    p.show(); print("  The Wraith lunges!\n", Color::RED); sleep_ms(300);

    Enemy wraith("Void Wraith", 120, 18, 100);
    auto res = combat(p, wraith);
    if (res == Result::DEAD) return false;

    clearScreen(); printHeader("VICTORY");
    print("  The Wraith dissolves to dust. Light floods in.", Color::GREEN);
    p.show();
    print("  Enemies slain : " + to_string(p.kills), Color::CYAN);
    print("  Gold collected: " + to_string(p.gold), Color::YELLOW);
    print("  HP remaining  : " + to_string(p.hp) + "/" + to_string(p.maxHp), Color::GREEN);
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    while (true) {
        clearScreen();
        cout << Color::GREEN << Color::BOLD;
        cout << R"(
  ╔══════════════════════════════════════════════════════╗
  ║        E C H O E S   O F   T H E   V O I D            ║
  ║              A Text Adventure in C++                 ║
  ╚══════════════════════════════════════════════════════╝
)" << Color::RESET;

        print("  Navigate 3 floors. Solve puzzles. Slay monsters.", Color::WHITE);
        print("  Defeat the Void Wraith to escape.\n", Color::WHITE);

        cout << "  [1] New Game\n  [2] Quit\n\n" << Color::GREEN << "  > " << Color::RESET;
        int choice; if (!(cin >> choice)) break; flushLine();
        if (choice == 2) { print("  Farewell.", Color::DIM); break; }

        Player p;
        cout << "\n  Enter your name: " << Color::GREEN; getline(cin, p.name); if (p.name.empty()) p.name = "Hero"; cout << Color::RESET;
        print("\n  Welcome, " + p.name + ". The dungeon awaits...\n", Color::CYAN, 200);

        bool survived = floor1(p);

        clearScreen();
        if (!survived) { printHeader("Y O U   D I E D"); print("  The darkness claims you.", Color::RED); p.show(); }

        print("\n  [1] Play Again   [2] Quit\n", Color::WHITE);
        cout << Color::GREEN << "  > " << Color::RESET;
        if (!(cin >> choice)) break; flushLine();
        if (choice == 2) { print("  Thanks for playing.", Color::DIM); break; }
    }
    return 0;
}
