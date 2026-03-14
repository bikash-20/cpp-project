#include <bits/stdc++.h>
using namespace std;

namespace Cfg 
{
    constexpr int kHpBarWidth = 20;
    constexpr int kHerbHeal = 35;
    constexpr bool kUseAnsi = true;
}
namespace Color 
{
    const string RESET   = "\033[0m";
    const string GREEN   = "\033[92m";
    const string YELLOW  = "\033[93m";
    const string RED     = "\033[91m";
    const string CYAN    = "\033[96m";
    const string BOLD    = "\033[1m";
    const string MAGENTA = "\033[95m";
}

struct Item
{
    string name;
    string type; 
    int value;   
    int price;

    void use(class Player& p); 
};

class Entity 
{
public:
    string name;
    int hp, maxHp, atk;

    Entity(string n, int h, int a) : name(n), hp(h), maxHp(h), atk(a) {}
    bool isAlive() const { return hp > 0; }
    virtual void takeDamage(int dmg) { hp = max(0, hp - dmg); }
};

class Player : public Entity 
{
public:
    int gold = 0, floor = 1, kills = 0;
    vector<Item> inventory;

    Player(string n) : Entity(n, 100, 12) {}

    void addItem(Item item) 
{
        inventory.push_back(item);
        cout << Color::YELLOW << "  [+] Received: " << item.name << Color::RESET << "\n";
    }

    void showStats() 
{
        cout << "\n" << Color::CYAN << "--- " << name << "'s Status ---" << Color::RESET << "\n";
        cout << "  HP: " << hp << "/" << maxHp << " | ATK: " << atk << " | Gold: " << gold << "\n";
        cout << "  Items: ";
        for(auto &i : inventory) cout << "[" << i.name << "] ";
        cout << "\n----------------------\n";
    }
};

void startBattle(Player& p, Entity& e) 
{
    cout << Color::RED << "\n  ⚠ " << e.name << " appears!" << Color::RESET << "\n";
    
    while(p.isAlive() && e.isAlive())
        {
        cout << "\n  1. Attack | 2. Heal | 3. Flee\n  > ";
        int choice; cin >> choice;

        if(choice == 1) 
        {
            int pDmg = p.atk + (rand() % 5);
            e.takeDamage(pDmg);
            cout << "  You dealt " << pDmg << " damage to " << e.name << "!\n";

            if(e.isAlive()) 
            {
                int eDmg = e.atk + (rand() % 3);
                p.takeDamage(eDmg);
                cout << "  " << e.name << " hit you for " << eDmg << "!\n";
            }
        } 
        else if(choice == 2) 
        {
            p.hp = min(p.maxHp, p.hp + Cfg::kHerbHeal);
            cout << "  You healed yourself!\n";
        } else break;

        p.showStats();
    }
}

int main() 
{
    srand(time(0));
    string pName;
    cout << Color::BOLD << "Enter Hero Name: " << Color::RESET;
    getline(cin, pName);

    Player player(pName);
    
    cout << "\n--- Floor 1: The Dark Cells ---\n";
    Entity slime("Acid Slime", 40, 5);
    startBattle(player, slime);

    if(player.isAlive())
    {
        player.kills++;
        player.gold += 20;
        Item rustySword = {"Rusty Sword", "Weapon", 5, 0};
        player.addItem(rustySword);
        player.atk += rustySword.value;
        cout << "  You found a Rusty Sword! Attack increased.\n";
    }

    if(!player.isAlive()) 
    {
        cout << Color::RED << "\n  GAME OVER. The Void consumed you." << Color::RESET << "\n";
    } 
    else
    {
        cout << Color::GREEN << "\n  VICTORY! You survived Floor 1." << Color::RESET << "\n";
        player.showStats();
    }

    return 0;
}
