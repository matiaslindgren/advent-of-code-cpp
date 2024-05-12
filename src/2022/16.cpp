#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Id {
  char a, b;
  explicit operator int() const {
    return (a - 'A') * ('Z' - 'A' + 1) + (b - 'A');
  }
};

struct Valve {
  int id{};
  int flow{};
  std::vector<int> tunnels;
};

using PairDist = std::vector<std::vector<int>>;

// https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
// accessed 2024-05-11
PairDist all_pairs_shortest_paths(auto valves) {
  const auto n{int{Id{'Z', 'Z'}} + 1};
  PairDist dist(n, std::vector<int>(n, std::numeric_limits<int>::max()));

  for (const Valve& v : valves) {
    for (int dst : v.tunnels) {
      dist[v.id][dst] = dist[dst][v.id] = 1;
    }
  }

  for (const Valve& v : valves) {
    dist[v.id][v.id] = 0;
  }

  for (const Valve& v1 : valves) {
    for (const Valve& v2 : valves) {
      for (const Valve& v3 : valves) {
        dist[v2.id][v3.id] = std::min(
            dist[v2.id][v3.id],
            aoc::saturating_add(dist[v2.id][v1.id], dist[v1.id][v3.id])
        );
      }
    }
  }

  return dist;
}

PairDist compress_distance_matrix(PairDist dist, const auto& subset) {
  const auto n{subset.size()};
  PairDist dist_subset(n, std::vector<int>(n, std::numeric_limits<int>::max()));
  for (auto [i1, v1] : my_std::views::enumerate(subset)) {
    for (auto [i2, v2] : my_std::views::enumerate(subset)) {
      dist_subset[i1][i2] = dist.at(v1.id).at(v2.id);
    }
  }
  return dist_subset;
}

using ValveMask = std::bitset<16>;

struct State {
  int position{};
  int time{};
  ValveMask opened{};
  int pressure{};

  bool operator==(const State& rhs) const {
    return position == rhs.position and time == rhs.time and opened == rhs.opened;
  }
};

template <>
struct std::hash<State> {
  std::size_t operator()(const State& s) const noexcept {
    auto h{s.opened.to_ulong()};
    h = (h << 4) | std::hash<int>{}(s.position);
    h = (h << 5) | std::hash<int>{}(s.time);
    return h;
  }
};

auto search_max_pressure(
    const auto& valves,
    const ValveMask& targets,
    const PairDist& dist,
    const int start_time
) {
  // how much lambda is too much lambda?
  return [&valves,
          &targets,
          &dist,
          cache = std::unordered_map<State, int>{}](this auto&& self, const State& s) {
    if (s.time <= 0) {
      return s.pressure;
    }
    if (cache.contains(s)) {
      return cache.at(s);
    }

    const int src{s.position};
    auto max_pressure{s.pressure};

    for (int dst{}; dst < valves.size(); ++dst) {
      if (targets[dst] and dst != src) {
        const int time_after_travel{s.time - dist[src][dst]};
        if (time_after_travel > 0) {
          max_pressure = std::max(
              max_pressure,
              self(State{
                  .position = dst,
                  .time = time_after_travel,
                  .opened = s.opened,
                  .pressure = s.pressure,
              })
          );
        }
      }
    }

    const Valve& valve{valves[src]};
    if (not s.opened[src] and valve.flow) {
      auto after_open{s.opened};
      after_open[src] = true;
      max_pressure = std::max(
          max_pressure,
          self(State{
              .position = src,
              .time = s.time - 1,
              .opened = after_open,
              .pressure = s.pressure + (s.time - 1) * valve.flow,
          })
      );
    }

    return (cache[s] = max_pressure);
  }(State{.position = int{Id{'A', 'A'}}, .time = start_time});
}

std::istream& operator>>(std::istream& is, Id& id) {
  if (char a, b; is >> a >> b) {
    if ('A' <= std::min(a, b) and std::max(a, b) <= 'Z') {
      id = {a, b};
    } else {
      throw std::runtime_error("invalid Id, should consist of two chars from [A-Z]");
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Valve& valve) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (Id id; ls >> skip("Valve"s) >> id) {
      if (int flow; ls >> std::ws >> skip("has flow rate="s) >> flow and flow >= 0) {
        if (std::string s1, s2, s3, s4; ls >> skip(";"s) >> s1 >> s2 >> s3 >> s4) {
          if ((s1 == "tunnels"s and s2 == "lead"s and s3 == "to"s and s4 == "valves")
              or (s1 == "tunnel"s and s2 == "leads"s and s3 == "to"s and s4 == "valve")) {
            std::vector<int> tunnels;
            for (Id tunnel; ls >> tunnel; ls >> skip(","s)) {
              tunnels.push_back(int{tunnel});
            }
            valve = Valve{int{id}, flow, tunnels};
          }
        }
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing valve from line '{}'", line));
    }
  }
  return is;
}

int main() {
  auto valves{aoc::slurp<Valve>("/dev/stdin")};

  const auto dist_all_paths{all_pairs_shortest_paths(valves)};

  std::erase_if(valves, [](const Valve& v) { return v.id != int{Id{'A', 'A'}} and v.flow <= 0; });
  const auto n_valves{ValveMask{}.size()};
  if (valves.size() != n_valves) {
    throw std::runtime_error(
        std::format("expected exactly {} valves with non-zero flow + starting valve", n_valves)
    );
  }

  ranges::sort(valves, {}, &Valve::id);
  const auto dist{compress_distance_matrix(dist_all_paths, valves)};

  const auto part1{search_max_pressure(valves, ~ValveMask{}, dist, 30)};

  int part2{};
  {
    std::unordered_map<ValveMask, int> best_subsets;
    for (unsigned long elephant{}; elephant < ((1 << n_valves) - 1); ++elephant) {
      ValveMask set1{elephant};
      ValveMask set2{~set1};
      if (std::min(set1.count(), set2.count()) < n_valves / 2) {
        // both units move at the same speed
        // => prune all branches with an unbalanced division of work
        continue;
      }

      set1[0] = true;
      if (not best_subsets.contains(set1)) {
        best_subsets[set1] = search_max_pressure(valves, set1, dist, 26);
      }

      set2[0] = true;
      if (not best_subsets.contains(set2)) {
        best_subsets[set2] = search_max_pressure(valves, set2, dist, 26);
      }

      part2 = std::max(part2, best_subsets.at(set1) + best_subsets.at(set2));
    }
  }

  std::print("{} {}\n", part1, part2);

  return 0;
}
