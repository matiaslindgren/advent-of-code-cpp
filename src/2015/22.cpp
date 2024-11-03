#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

struct Unit {
  int hp;
  int mana;
  int armor;
  int damage;
  int shield_left;
  int poison_left;
  int recharge_left;
  int total_spent_mana;

  [[nodiscard]]
  Unit set(int Unit::* member, int value) const {
    Unit u{*this};
    u.*member = value;
    return u;
  }

  [[nodiscard]]
  Unit use_mana(int m) const {
    return set(&Unit::mana, mana - m).set(&Unit::total_spent_mana, total_spent_mana + m);
  }

  [[nodiscard]]
  Unit set_recharge(int r) const {
    return set(&Unit::recharge_left, r);
  }

  [[nodiscard]]
  Unit set_poison(int p) const {
    return set(&Unit::poison_left, p);
  }

  [[nodiscard]]
  Unit set_shield(int s) const {
    return set(&Unit::shield_left, s);
  }

  [[nodiscard]]
  Unit set_hp(int h) const {
    return set(&Unit::hp, h);
  }

  [[nodiscard]]
  Unit take_damage(int d) const {
    return set_hp(hp + std::min(-1, armor - d));
  }
};

struct State {
  Unit player;
  Unit boss;
  bool player_turn;

  [[nodiscard]]
  auto transitions() const {
    if (not player_turn) {
      return std::vector{State{player.take_damage(boss.damage), boss, true}};
    }
    std::vector<State> ts;
    if (player.mana >= 229 and player.recharge_left == 0) {
      ts.push_back({player.use_mana(229).set_recharge(5), boss});
    }
    if (player.mana >= 173 and player.poison_left == 0) {
      ts.push_back({player.use_mana(173).set_poison(6), boss});
    }
    if (player.mana >= 113 and player.shield_left == 0) {
      ts.push_back({player.use_mana(113).set_shield(6), boss});
    }
    if (player.mana >= 73) {
      ts.push_back({player.use_mana(73).set_hp(player.hp + 2), boss.set_hp(boss.hp - 2)});
    }
    if (player.mana >= 53) {
      ts.push_back({player.use_mana(53), boss.set_hp(boss.hp - 4)});
    }
    return ts;
  }
};

int find_min_win_mana(const Unit& boss, const int player_poison = 0) {
  int min_win_mana{std::numeric_limits<int>::max()};
  for (std::vector q{State{{50, 500}, boss, true}}; not q.empty();) {
    State s{q.back()};
    q.pop_back();
    if (min_win_mana < s.player.total_spent_mana) {
      continue;
    }
    s.player.armor = 0;
    if (s.player_turn) {
      s.player.hp -= player_poison;
    }
    if (s.player.shield_left > 0) {
      s.player.armor += 7;
      s.player.shield_left -= 1;
    }
    if (s.player.poison_left > 0) {
      s.boss.hp -= 3;
      s.player.poison_left -= 1;
    }
    if (s.player.recharge_left > 0) {
      s.player.mana += 101;
      s.player.recharge_left -= 1;
    }
    if (s.player.hp <= 0) {
      continue;
    }
    if (s.boss.hp <= 0) {
      min_win_mana = std::min(min_win_mana, s.player.total_spent_mana);
      continue;
    }
    q.append_range(s.transitions());
  }
  return min_win_mana;
}

int main() {
  std::istringstream is{aoc::slurp_file("/dev/stdin")};
  if (Unit boss{}; is >> std::ws >> skip("Hit"s, "Points:"s) >> boss.hp >> std::ws
                   >> skip("Damage:"s) >> boss.damage) {
    const auto part1{find_min_win_mana(boss)};
    const auto part2{find_min_win_mana(boss, 1)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing boss");
}
