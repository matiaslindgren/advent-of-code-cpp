#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

struct Graph {
  std::unordered_map<std::string, std::vector<std::string>> edges;

  void add_edge(auto src, auto dst) {
    auto& e{edges[src]};
    if (not ranges::contains(e, dst)) {
      e.push_back(dst);
    }
  }
};

enum class Search : unsigned char {
  simple,
  recursive,
};

auto search(Graph g, Search type) {
  int path_count{};

  const auto small_caves{
      g.edges | views::keys | views::filter([](auto s) {
        return ranges::all_of(s, [](unsigned char c) { return std::islower(c); });
      })
      | ranges::to<std::unordered_set>()
  };

  for (std::vector q{std::tuple{
           "start"s,
           type == Search::simple,
           std::unordered_set<std::string>{},
       }};
       not q.empty();) {
    auto [cave, small_twice, visited]{q.back()};
    q.pop_back();

    if (cave == "end"s) {
      path_count += 1;
      continue;
    }

    if (auto&& [_, unseen]{visited.insert(cave)}; not unseen) {
      if (cave == "start"s) {
        continue;
      }
      if (small_caves.contains(cave)) {
        if (small_twice) {
          continue;
        }
        small_twice = true;
      }
    }

    for (const auto& next : g.edges.at(cave)) {
      q.emplace_back(next, small_twice, visited);
    }
  }

  return path_count;
}

Graph parse_input(std::string_view path) {
  Graph g;
  for (std::string line : aoc::slurp_lines(path)) {
    ranges::replace(line, '-', ' ');
    std::istringstream ls{line};
    if (std::string src, dst; ls >> src >> dst and ls.eof()) {
      g.add_edge(src, dst);
      g.add_edge(dst, src);
    } else {
      throw std::runtime_error(std::format("failed parsing line '{}'", line));
    }
  }
  return g;
}

int main() {
  const Graph graph{parse_input("/dev/stdin")};

  const auto part1{search(graph, Search::simple)};
  const auto part2{search(graph, Search::recursive)};

  std::println("{} {}", part1, part2);

  return 0;
}
