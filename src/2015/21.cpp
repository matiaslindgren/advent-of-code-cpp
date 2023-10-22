import std;

namespace ranges = std::ranges;
namespace views = std::views;

/*
Weapons:    Cost  Damage  Armor
Dagger        8     4       0
Shortsword   10     5       0
Warhammer    25     6       0
Longsword    40     7       0
Greataxe     74     8       0

Armor:      Cost  Damage  Armor
Leather      13     0       1
Chainmail    31     0       2
Splintmail   53     0       3
Bandedmail   75     0       4
Platemail   102     0       5

Rings:      Cost  Damage  Armor
Damage +1    25     1       0
Damage +2    50     2       0
Damage +3   100     3       0
Defense +1   20     0       1
Defense +2   40     0       2
Defense +3   80     0       3
*/

struct Weapon {
  enum : int {
    Dagger,
    Shortsword,
    Warhammer,
    Longsword,
    Greataxe,
  } type;
  int cost;
  int damage;
  int armor;
};

struct Armor {
  enum : int {
    None,
    Leather,
    Chainmail,
    Splintmail,
    Bandedmail,
    Platemail,
  } type;
  int cost;
  int damage;
  int armor;
};

struct Ring {
  enum : int {
    None,
    Damage1,
    Damage2,
    Damage3,
    Defense1,
    Defense2,
    Defense3,
  } type;
  int cost;
  int damage;
  int armor;
};

static const std::vector<Weapon> weapons{
    {    Weapon::Dagger,  8, 4, 0},
    {Weapon::Shortsword, 10, 5, 0},
    { Weapon::Warhammer, 25, 6, 0},
    { Weapon::Longsword, 40, 7, 0},
    {  Weapon::Greataxe, 74, 8, 0},
};

static const std::vector<Armor> armor{
    {      Armor::None,   0, 0, 0},
    {   Armor::Leather,  13, 0, 1},
    { Armor::Chainmail,  31, 0, 2},
    {Armor::Splintmail,  53, 0, 3},
    {Armor::Bandedmail,  75, 0, 4},
    { Armor::Platemail, 102, 0, 5},
};

static const std::vector<Ring> rings{
    {    Ring::None,   0, 0, 0},
    { Ring::Damage1,  25, 1, 0},
    { Ring::Damage2,  50, 2, 0},
    { Ring::Damage3, 100, 3, 0},
    {Ring::Defense1,  20, 0, 1},
    {Ring::Defense2,  40, 0, 2},
    {Ring::Defense3,  80, 0, 3},
};

struct Boss {
  int hp;
  int damage;
  int armor;
};

std::istream& operator>>(std::istream& is, Boss& boss) {
  std::string tmp;
  int hp, damage, armor;
  if (is >> tmp && tmp == "Hit" && is >> tmp && tmp == "Points:" && is >> hp >> tmp
      && tmp == "Damage:" && is >> damage >> tmp && tmp == "Armor:" && is >> armor) {
    boss = {hp, damage, armor};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Boss");
}

struct Player {
  Weapon weapon;
  Armor armor;
  Ring ring1;
  Ring ring2;
  int hp = 100;

  int total_damage() const {
    return weapon.damage + armor.damage + ring1.damage + ring2.damage;
  }
  int total_armor() const {
    return weapon.armor + armor.armor + ring1.armor + ring2.armor;
  }
  int total_cost() const {
    return weapon.cost + armor.cost + ring1.cost + ring2.cost;
  }
  bool wins(const Boss& boss) const {
    const int delta_hp_p{std::min(-1, total_armor() - boss.damage)};
    const int delta_hp_b{std::min(-1, boss.armor - total_damage())};
    int hp_p{hp};
    int hp_b{boss.hp};
    for (bool my_turn{true}; hp_p > 0 && hp_b > 0; my_turn = !my_turn) {
      if (my_turn) {
        hp_b += delta_hp_b;
      } else {
        hp_p += delta_hp_p;
      }
    }
    return hp_p > 0;
  }
};

std::pair<int, int> minmax_gold(const Boss& boss) {
  int min_win_gold{std::numeric_limits<int>::max()};
  int max_lose_gold{0};
  for (const auto& w : weapons) {
    for (const auto& a : armor) {
      for (const auto& r1 : rings) {
        for (const auto& r2 : rings) {
          if (r1.type == r2.type && r1.type != Ring::None) {
            continue;
          }
          const Player player = {w, a, r1, r2};
          if (player.wins(boss)) {
            min_win_gold = std::min(min_win_gold, player.total_cost());
          } else {
            max_lose_gold = std::max(max_lose_gold, player.total_cost());
          }
        }
      }
    }
  }
  return {min_win_gold, max_lose_gold};
}

int main() {
  std::ios_base::sync_with_stdio(false);

  Boss boss;
  std::cin >> boss;

  const auto [part1, part2] = minmax_gold(boss);
  std::print("{} {}\n", part1, part2);

  return 0;
}
