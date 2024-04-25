#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Flavour : char {
  bludgeoning = 'b',
  cold = 'c',
  fire = 'f',
  radiation = 'r',
  slashing = 's',
};

using Flavours = std::vector<Flavour>;

struct Group {
  int units{}, hp{}, power{}, init{};
  Flavours weak_to, immune_to;
  Flavour attacks_with;

  bool is_weak_to(Flavour f) const {
    return ranges::contains(weak_to, f);
  }

  bool is_immune_to(Flavour f) const {
    return ranges::contains(immune_to, f);
  }

  int damage_to(const Group& target) const {
    const int multiplier{1 + target.is_weak_to(attacks_with) - target.is_immune_to(attacks_with)};
    return units * power * multiplier;
  }

  void attack(Group& target) const {
    target.units -= std::min(target.units, damage_to(target) / target.hp);
  }
};

std::istream& operator>>(std::istream& is, Flavours& fs) {
  for (std::underlying_type_t<Flavour> ch; is >> ch and ch != ';';) {
    Flavour f;
    switch (ch) {
      case 'b': {
        if (is >> skip("ludgeoning"s)) {
          f = {ch};
        }
      } break;
      case 'c': {
        if (is >> skip("old"s)) {
          f = {ch};
        }
      } break;
      case 'f': {
        if (is >> skip("ire"s)) {
          f = {ch};
        }
      } break;
      case 'r': {
        if (is >> skip("adiation"s)) {
          f = {ch};
        }
      } break;
      case 's': {
        if (is >> skip("lashing"s)) {
          f = {ch};
        }
      } break;
      default: {
        is.setstate(std::ios_base::failbit);
      }
    }
    if (is) {
      fs.push_back(f);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Flavours");
}

std::istream& operator>>(std::istream& is, Group& group) {
  if (std::string line; is >> std::ws and std::getline(is, line) and not line.empty()) {
    ranges::replace(line, ')', ';');
    ranges::replace(line, '(', ' ');
    ranges::replace(line, ',', ' ');
    line.insert(line.find("damage"s), "; "s);

    std::istringstream ls{line};
    if (Group g; ls >> g.units >> std::ws >> skip("units each with"s) >> g.hp >> std::ws
                 >> skip("hit points"s)) {
      if (std::string s; ls >> s) {
        if (s == "weak"s) {
          if (ls >> std::ws >> skip("to"s) >> g.weak_to) {
            ls >> s;
          }
        }

        if (s == "immune"s) {
          if (ls >> std::ws >> skip("to"s) >> g.immune_to) {
            if (ls >> s and s == "weak"s) {
              if (ls >> std::ws >> skip("to"s) >> g.weak_to) {
                ls >> s;
              }
            }
          }
        }

        if (s == "with"s) {
          if (ls >> std::ws >> skip("an attack that does"s) >> g.power) {
            if (Flavours fs; ls >> fs and fs.size() == 1) {
              g.attacks_with = fs.front();
              if (ls >> std::ws >> skip("damage at initiative"s) >> g.init) {
                group = g;
              }
            }
          }
        } else {
          ls.setstate(std::ios_base::failbit);
        }
      }
    }

    if (not ls.eof()) {
      is.setstate(std::ios_base::failbit);
    }
  }

  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Group");
}

using Groups = std::vector<Group>;

std::array<Groups, 2> parse_input(std::string path) {
  const auto input{aoc::slurp_file(path)};
  const auto sections{
      views::split(input, "\n\n"s)
      | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
      | ranges::to<std::vector<std::string>>()
  };
  if (sections.size() != 2) {
    throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
  }

  if (std::istringstream is{sections[0]}; is >> skip("Immune System:"s)) {
    Groups immune_sys{views::istream<Group>(is) | ranges::to<std::vector>()};
    if (is.eof()) {
      is = std::istringstream(sections[1]);
      if (is >> skip("Infection:"s)) {
        Groups infection{views::istream<Group>(is) | ranges::to<std::vector>()};
        if (is.eof()) {
          return {immune_sys, infection};
        }
      }
    }
  }

  throw std::runtime_error("failed parsing input sections");
}

bool fight(auto& teams) {
  const auto get_group{[&teams](auto&& team, auto&& id) -> auto&& { return teams[team][id]; }};

  using GroupID = std::pair<std::size_t, std::size_t>;

  std::vector<GroupID> select_q;
  std::array<std::vector<std::size_t>, 2> available;

  for (auto team{0uz}; team < 2; ++team) {
    for (auto id{0uz}; id < teams[team].size(); ++id) {
      select_q.emplace_back(team, id);
      if (get_group(team, id).units) {
        available[team].push_back(id);
      }
    }
  }

  ranges::sort(select_q, {}, [&](GroupID gid) {
    auto [team, id]{gid};
    const Group& g{get_group(team, id)};
    return std::tuple{g.units * g.power, g.init};
  });

  std::vector<std::pair<GroupID, GroupID>> attack_q;

  for (; not select_q.empty(); select_q.pop_back()) {
    const auto& [team1, id1]{select_q.back()};
    const auto team2{(team1 + 1) % 2};

    const auto id2{ranges::max_element(available[team2], {}, [&](auto&& id2) {
      const Group& g1{get_group(team1, id1)};
      const Group& g2{get_group(team2, id2)};
      return std::tuple{g1.damage_to(g2), g2.units * g2.power, g2.init};
    })};

    if (id2 != available[team2].end()) {
      const Group& g1{get_group(team1, id1)};
      const Group& g2{get_group(team2, *id2)};
      if (g1.damage_to(g2)) {
        attack_q.emplace_back(GroupID{team1, id1}, GroupID{team2, *id2});
        available[team2].erase(id2);
      }
    }
  }

  ranges::sort(attack_q, {}, [&](auto&& ab) {
    auto&& [a, _] = ab;
    auto [team, id]{a};
    return get_group(team, id).init;
  });

  int total_dead{};

  for (; not attack_q.empty(); attack_q.pop_back()) {
    auto&& [a, b]{attack_q.back()};
    auto&& [team1, id1]{a};
    auto&& [team2, id2]{b};

    const Group& g1{get_group(team1, id1)};
    Group& g2{get_group(team2, id2)};

    if (g1.units) {
      const auto u2{g2.units};
      g1.attack(g2);
      total_dead += u2 - g2.units;
    }
  }

  return total_dead > 0;
}

inline constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

auto count_units(const auto& teams) {
  return std::pair{
      sum(views::transform(teams[0], &Group::units)),
      sum(views::transform(teams[1], &Group::units))
  };
}

auto find_part1(const auto& input_teams) {
  for (auto teams{input_teams}; fight(teams);) {
    auto&& [u1, u2]{count_units(teams)};
    if (not u1) {
      return u2;
    }
    if (not u2) {
      return u1;
    }
  }
  throw std::runtime_error("unexpected stalemate during part1");
}

auto find_part2(const auto& input_teams) {
  for (int boost{}; boost < 1'000; ++boost) {
    auto teams{input_teams};
    ranges::for_each(teams[0], [=](Group& g) { g.power += boost; });
    for (; fight(teams);) {
      auto&& [u1, u2]{count_units(teams)};
      if (not u1) {
        break;
      }
      if (not u2) {
        return u1;
      }
    }
  }
  throw std::runtime_error("search space exhausted during part2");
}

int main() {
  const auto teams{parse_input("/dev/stdin")};

  const auto part1{find_part1(teams)};
  const auto part2{find_part2(teams)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
