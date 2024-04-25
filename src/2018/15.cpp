#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class Tile : char {
  wall = '#',
  open = '.',
  goblin = 'G',
  elf = 'E',
};

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (std::underlying_type_t<Tile> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Tile::wall):
      case std::to_underlying(Tile::open):
      case std::to_underlying(Tile::goblin):
      case std::to_underlying(Tile::elf): {
        tile = {ch};
      } break;
      default: {
        is.setstate(std::ios_base::failbit);
      } break;
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Tile");
}

struct Unit {
  Tile type;
  int id{}, power{}, hp{};
};

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};
using Vec2 = aoc::Vec2<int>;

struct Cave {
  int width{};
  std::vector<bool> walls;
  std::vector<Unit> units;
  std::vector<Vec2> state;

  auto size() const {
    return walls.size();
  }

  auto index(const Vec2& p) const {
    return p.y() * width + p.x();
  }

  auto&& get_pos(this auto&& self, const Unit& u) {
    return self.state.at(u.id);
  }

  bool is_wall(const Vec2& p) const {
    return walls.at(index(p));
  }

  auto alive_units() {
    return views::filter(units, [&](auto& u) { return u.hp > 0; });
  }

  auto team_hp(Tile type) {
    return sum(
        alive_units() | views::filter([&](auto&& u) { return u.type == type; })
        | views::transform(&Unit::hp)
    );
  }

  auto find_opponents(const Unit& u1) {
    return views::filter(alive_units(), [&u1](auto& u2) { return u1.type != u2.type; });
  }

  auto find_adjacent_opponents(const Unit& u1) {
    return views::filter(find_opponents(u1), [&](auto& u2) {
      return get_pos(u1).distance(get_pos(u2)) == 1;
    });
  }

  auto find_target_paths(const Unit& unit) {
    const auto blocked{
        views::transform(alive_units(), [&](auto&& u) { return index(get_pos(u)); })
        | ranges::to<std::unordered_set>()
    };

    std::unordered_set<int> targets;
    for (const Unit& opp : find_opponents(unit)) {
      const auto pos{get_pos(opp)};
      for (Vec2 target : pos.adjacent()) {
        if (not is_wall(target)) {
          if (const auto idx{index(target)}; not blocked.contains(idx)) {
            targets.insert(idx);
          }
        }
      }
    }

    std::vector<std::vector<Vec2>> paths;
    {
      std::unordered_set<int> visited;
      for (auto q{
               get_pos(unit).adjacent() | views::transform([](auto&& p) { return std::vector{p}; })
               | ranges::to<std::deque>()
           };
           not q.empty();
           q.pop_front()) {
        const auto& path{q.front()};
        const auto pos{path.back()};

        if (is_wall(pos)) {
          continue;
        }

        const auto idx{index(pos)};
        if (blocked.contains(idx)) {
          continue;
        }
        if (targets.contains(idx)) {
          paths.push_back(path);
          continue;
        }

        if (const auto& [_, unseen]{visited.insert(idx)}; unseen) {
          for (auto adj : pos.adjacent()) {
            auto p{path};
            p.push_back(adj);
            q.push_back(p);
          }
        }
      }
    }
    return paths;
  }
};

Cave parse_cave(std::string path) {
  Cave g{};
  std::istringstream is{aoc::slurp_file(path)};
  {
    Vec2 p(0, 0);
    int unit_id{};
    for (std::string line; std::getline(is, line) and not line.empty(); ++p.y()) {
      if (not g.width) {
        g.width = line.size();
      } else if (line.size() != g.width) {
        is.setstate(std::ios_base::failbit);
        break;
      }

      std::stringstream ls{line};
      p.x() = 0;
      for (Tile t : views::istream<Tile>(ls)) {
        g.walls.push_back(t == Tile::wall);
        if (t == Tile::goblin or t == Tile::elf) {
          g.units.push_back(Unit{
              .type = t,
              .id = unit_id++,
              .power = 3,
              .hp = 200,
          });
          g.state.push_back(p);
        }
        ++p.x();
      }

      if (not ls.eof()) {
        is.setstate(std::ios_base::failbit);
      }
    }
  }
  if (is.eof()) {
    return g;
  }

  throw std::runtime_error("failed parsing Cave");
}

std::optional<int> simulate(Cave cave, const bool is_part2 = false) {
  const auto unit_priority{[&cave](const Unit& u) {
    const auto& pos{cave.get_pos(u)};
    return std::tuple{pos.y(), pos.x()};
  }};

  const auto target_priority{[&unit_priority](const Unit& u) {
    return std::tuple_cat(std::tuple{u.hp}, unit_priority(u));
  }};

  const auto path_priority{[&cave](const auto& path) {
    const auto target{cave.index(path.back())};
    const auto step{cave.index(path.front())};
    return std::tuple{path.size(), target, step};
  }};

  for (int round{};; ++round) {
    cave.units = cave.alive_units() | ranges::to<std::vector>();
    ranges::sort(cave.units, {}, unit_priority);

    for (auto& unit : cave.units) {
      if (unit.hp <= 0) {
        continue;
      }

      if (ranges::empty(cave.find_opponents(unit))) {
        return round * cave.team_hp(unit.type);
      }

      auto targets{cave.find_adjacent_opponents(unit)};

      if (targets.empty()) {
        if (auto paths{cave.find_target_paths(unit)}; not paths.empty()) {
          const auto min_path{ranges::min(paths, {}, path_priority)};
          cave.get_pos(unit) = min_path.front();
          targets = cave.find_adjacent_opponents(unit);
        }
      }

      if (not targets.empty()) {
        auto target{ranges::min_element(targets, {}, target_priority)};
        target->hp -= unit.power;
        if (target->hp < 0 and target->type == Tile::elf and is_part2) {
          return {};
        }
      }
    }
  }
}

auto find_part1(Cave cave) {
  return simulate(cave).value_or(0);
}

auto find_part2(Cave cave) {
  for (int power{4};; ++power) {
    ranges::for_each(cave.units, [=](Unit& u) {
      if (u.type == Tile::elf) {
        u.power = power;
      }
    });
    if (const auto outcome{simulate(cave, true)}) {
      return *outcome;
    }
  }
}

int main() {
  const auto cave{parse_cave("/dev/stdin")};

  const auto part1{find_part1(cave)};
  const auto part2{find_part2(cave)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
