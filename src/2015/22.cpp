import std;

struct Boss {
  int hp;
  int damage;
};

std::istream& operator>>(std::istream& is, Boss& boss) {
  std::string tmp;
  int hp, damage;
  if (is >> tmp && tmp == "Hit" && is >> tmp && tmp == "Points:" && is >> hp >> tmp
      && tmp == "Damage:" && is >> damage) {
    boss = {hp, damage};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Boss");
}

struct Player {
  int hp;
  int mana;
  int armor;
  int shield_left;
  int poison_left;
  int recharge_left;
  int total_spent_mana;

  void use_mana(int m) {
    mana -= m;
    total_spent_mana += m;
  }
};

struct State {
  Player player;
  Boss boss;
  bool player_turn;
};

int find_min_win_mana(const Boss& boss, const int player_poison = 0) {
  int min_win_mana{std::numeric_limits<int>::max()};

  for (std::vector<State> q = {{.player = {50, 500}, .boss = boss, .player_turn = true}};
       !q.empty();) {
    State state = q.back();
    q.pop_back();

    if (min_win_mana < state.player.total_spent_mana) {
      continue;
    }

    Player p = state.player;
    p.armor = 0;
    Boss b = state.boss;

    if (state.player_turn) {
      p.hp -= player_poison;
    }

    if (p.shield_left) {
      p.armor += 7;
      --p.shield_left;
    }
    if (p.poison_left) {
      b.hp -= 3;
      --p.poison_left;
    }
    if (p.recharge_left) {
      p.mana += 101;
      --p.recharge_left;
    }

    if (p.hp <= 0) {
      continue;
    }
    if (b.hp <= 0) {
      min_win_mana = std::min(min_win_mana, p.total_spent_mana);
      continue;
    }

    if (!state.player_turn) {
      Player p2 = p;
      Boss b2 = b;
      p2.hp += std::min(-1, p2.armor - b2.damage);
      q.push_back({.player = p2, .boss = b2, .player_turn = true});
      continue;
    }

    if (p.mana >= 229 && !p.recharge_left) {
      Player p2 = p;
      p2.use_mana(229);
      p2.recharge_left = 5;
      q.push_back({p2, b});
    }
    if (p.mana >= 173 && !p.poison_left) {
      Player p2 = p;
      p2.use_mana(173);
      p2.poison_left = 6;
      q.push_back({p2, b});
    }
    if (p.mana >= 113 && !p.shield_left) {
      Player p2 = p;
      p2.use_mana(113);
      p2.shield_left = 6;
      q.push_back({p2, b});
    }
    if (p.mana >= 73) {
      Player p2 = p;
      Boss b2 = b;
      p2.use_mana(73);
      p2.hp += 2;
      b2.hp -= 2;
      q.push_back({p2, b2});
    }
    if (p.mana >= 53) {
      Player p2 = p;
      Boss b2 = b;
      p2.use_mana(53);
      b2.hp -= 4;
      q.push_back({p2, b2});
    }
  }

  return min_win_mana;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  Boss boss;
  std::cin >> boss;

  const auto part1{find_min_win_mana(boss)};
  const auto part2{find_min_win_mana(boss, 1)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
