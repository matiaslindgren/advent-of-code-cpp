#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Rocks {
  int ore{};
  int clay{};
  int obsidian{};
  int geodes{};

  int min() const {
    return std::min({ore, clay, obsidian, geodes});
  }

  auto operator<=>(const Rocks&) const = default;
};

struct Blueprint {
  int id{};
  Rocks ore_robot_cost{};
  Rocks clay_robot_cost{};
  Rocks obsidian_robot_cost{};
  Rocks geode_robot_cost{};

  int max_ore() const {
    return std::max({
        ore_robot_cost.ore,
        clay_robot_cost.ore,
        obsidian_robot_cost.ore,
        geode_robot_cost.ore,
    });
  }
};

auto div_up_or_zero(auto a, auto b) {
  return b ? (a + b - 1) / b : 0;
}

struct State {
  int time{};
  Rocks inventory{};
  Rocks producing{};

 private:
  State build_robot(const Rocks& cost, const Rocks& new_prod) const {
    int wait_time{
        1
        + std::max({
            div_up_or_zero(std::max(0, cost.ore - inventory.ore), producing.ore),
            div_up_or_zero(std::max(0, cost.clay - inventory.clay), producing.clay),
            div_up_or_zero(std::max(0, cost.obsidian - inventory.obsidian), producing.obsidian),
        })
    };
    return State{
        .time = time - wait_time,
        .inventory = {
          inventory.ore + wait_time * producing.ore - cost.ore,
          inventory.clay + wait_time * producing.clay - cost.clay,
          inventory.obsidian + wait_time * producing.obsidian - cost.obsidian,
          inventory.geodes,
        },
        .producing = {
          producing.ore + new_prod.ore,
          producing.clay + new_prod.clay,
          producing.obsidian + new_prod.obsidian,
        },
    };
  }

 public:
  State build_ore_robot(const Blueprint& bp) const {
    return build_robot(bp.ore_robot_cost, Rocks{.ore = 1});
  }

  State build_clay_robot(const Blueprint& bp) const {
    return build_robot(bp.clay_robot_cost, Rocks{.clay = 1});
  }

  State build_obsidian_robot(const Blueprint& bp) const {
    return build_robot(bp.obsidian_robot_cost, Rocks{.obsidian = 1});
  }

  State make_geodes(const Blueprint& bp) const {
    State s{build_robot(bp.geode_robot_cost, {})};
    s.inventory.geodes += s.time;
    return s;
  }

  bool excessive_production(const Blueprint& bp) const {
    return (producing.obsidian > bp.geode_robot_cost.obsidian)
           or (producing.clay > bp.obsidian_robot_cost.clay) or (producing.ore > bp.max_ore());
  }

  bool is_valid() const {
    return std::min(time, inventory.min()) >= 0;
  }

  int maximum_possible_geodes() const {
    return inventory.geodes + (time - 1) * time / 2;
  }

  auto operator<=>(const State&) const = default;
};

template <>
struct std::hash<State> {
  std::size_t operator()(const State& s) const noexcept {
    auto hash_int{std::hash<int>{}};
    auto h{hash_int(s.time)};
    h = (h << 10) | hash_int(s.inventory.ore);
    h = (h << 10) | hash_int(s.inventory.clay);
    h = (h << 10) | hash_int(s.inventory.obsidian);
    h = (h << 4) | hash_int(s.inventory.geodes);
    h = (h << 8) | hash_int(s.producing.ore);
    h = (h << 8) | hash_int(s.producing.clay);
    h = (h << 8) | hash_int(s.producing.obsidian);
    return h;
  }
};

auto find_max_geodes(Blueprint bp, int time_limit) {
  int max_n_geodes{};
  {
    std::unordered_set<State> visited;
    for (std::vector q{State{.time = time_limit, .producing = {.ore = 1}}}; not q.empty();) {
      State s{q.back()};
      q.pop_back();

      max_n_geodes = std::max(max_n_geodes, s.inventory.geodes);

      if (s.time < 2 or s.maximum_possible_geodes() < max_n_geodes) {
        continue;
      }

      std::vector<State> next_states;
      if (s.producing.ore) {
        next_states.push_back(s.build_ore_robot(bp));
        next_states.push_back(s.build_clay_robot(bp));
        if (s.producing.clay) {
          next_states.push_back(s.build_obsidian_robot(bp));
        }
        if (s.producing.obsidian) {
          next_states.push_back(s.make_geodes(bp));
        }
      }

      for (const State& s2 : next_states) {
        if (s2.is_valid() and not s2.excessive_production(bp)) {
          if (const auto [_, is_new]{visited.insert(s2)}; is_new) {
            q.push_back(s2);
          }
        }
      }
    }
  }
  return max_n_geodes;
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};
constexpr auto product{std::__bind_back(ranges::fold_left, 1, std::multiplies{})};

auto find_part1(auto blueprints) {
  return sum(views::transform(blueprints, [](const Blueprint& blueprint) {
    return blueprint.id * find_max_geodes(blueprint, 24);
  }));
}

auto find_part2(auto blueprints) {
  ranges::sort(blueprints, {}, &Blueprint::id);
  return product(views::transform(blueprints | views::take(3), [](const Blueprint& blueprint) {
    return find_max_geodes(blueprint, 32);
  }));
}

std::istream& operator>>(std::istream& is, Rocks& rocks) {
  bool ok{false};
  for (auto [kind, amount]{std::pair{""s, int{}}};
       is >> amount >> kind and amount > 0 and not kind.empty();) {
    if (kind.starts_with("ore"s)) {
      rocks.ore = amount;
    } else if (kind.starts_with("clay"s)) {
      rocks.clay = amount;
    } else if (kind.starts_with("obsidian"s)) {
      rocks.obsidian = amount;
    }
    if (kind.ends_with("."s)) {
      ok = true;
      break;
    } else if (not(is >> kind and kind == "and"s)) {
      break;
    }
  }
  if (not ok and not is.eof()) {
    throw std::runtime_error("failed parsing Rocks");
  }
  return is;
}

std::istream& operator>>(std::istream& is, Blueprint& bp) {
  bool ok{false};
  if (int id; is >> std::ws >> skip("Blueprint"s) >> id >> skip(":"s)) {
    if (Rocks ore; is >> std::ws >> skip("Each ore robot costs"s) >> ore) {
      if (Rocks clay; is >> std::ws >> skip("Each clay robot costs"s) >> clay) {
        if (Rocks obsidian; is >> std::ws >> skip("Each obsidian robot costs"s) >> obsidian) {
          if (Rocks geodes; is >> std::ws >> skip("Each geode robot costs"s) >> geodes) {
            bp = {id, ore, clay, obsidian, geodes};
            ok = true;
          }
        }
      }
    }
  }
  if (not ok and not is.eof()) {
    throw std::runtime_error("failed parsing Blueprint");
  }
  return is;
}

int main() {
  const auto blueprints{aoc::slurp<Blueprint>("/dev/stdin")};

  const auto part1{find_part1(blueprints)};
  const auto part2{find_part2(blueprints)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
