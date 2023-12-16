import std;

struct Item {
  int cost;
  int damage;
  int armor;
  constexpr auto operator<=>(const Item&) const = default;
};

static const std::vector<Item> weapons{
    {8, 4, 0},
    {10, 5, 0},
    {25, 6, 0},
    {40, 7, 0},
    {74, 8, 0},
};

static const std::vector<Item> armor{
    {0, 0, 0},
    {13, 0, 1},
    {31, 0, 2},
    {53, 0, 3},
    {75, 0, 4},
    {102, 0, 5},
};

static const std::vector<Item> rings{
    {0, 0, 0},
    {25, 1, 0},
    {50, 2, 0},
    {100, 3, 0},
    {20, 0, 1},
    {40, 0, 2},
    {80, 0, 3},
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
  Item weapon;
  Item armor;
  Item ring1;
  Item ring2;
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
    const int player_hp_delta{std::min(-1, total_armor() - boss.damage)};
    const int boss_hp_delta{std::min(-1, boss.armor - total_damage())};
    return hp * player_hp_delta >= boss.hp * boss_hp_delta;
  }
};

std::pair<int, int> minmax_gold(const Boss& boss) {
  int min_win_gold{std::numeric_limits<int>::max()};
  int max_lose_gold{0};
  for (const auto& w : weapons) {
    for (const auto& a : armor) {
      for (const auto& r1 : rings) {
        for (const auto& r2 : rings) {
          if (r1 == r2 && !r1.cost) {
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
