#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using std::operator""s;
using aoc::skip;
using Vec2 = ndvec::vec2<int>;

namespace ranges = std::ranges;
namespace views = std::views;

struct Node {
  Vec2 pos;
  int size{};
  int used{};

  auto operator<=>(const Node&) const = default;
};

auto count_viable_pairs(const auto& nodes) {
  return ranges::count_if(
      my_std::views::cartesian_product(nodes, nodes),
      my_std::apply_fn([](const Node& na, const Node& nb) {
        return (na != nb) and (na.used > 0) and (na.used + nb.used <= nb.size);
      })
  );
}

auto find_shortest_path(const auto& nodes) {
  const auto x_max{ranges::max(views::transform(nodes, [](const Node& n) { return n.pos.x(); }))};

  std::unordered_map<Vec2, std::unordered_set<Vec2>> visited;
  std::unordered_map<Vec2, int> used;
  std::unordered_map<Vec2, int> size;

  Vec2 free_pos;
  for (const Node& n : nodes) {
    if (n.used == 0) {
      free_pos = n.pos;
    }
    used[n.pos] = n.used;
    size[n.pos] = n.size;
  }

  for (std::deque q{std::tuple{Vec2(x_max, 0), free_pos, 0U}}; not q.empty(); q.pop_front()) {
    auto [data_pos, free_pos, steps]{q.front()};
    if (data_pos == Vec2()) {
      return steps;
    }
    if (auto [_, is_new]{visited[data_pos].insert(free_pos)}; is_new) {
      for (Vec2 adj : free_pos.adjacent()) {
        if (size[adj] > 0 and used[adj] <= size[free_pos]) {
          q.emplace_back(data_pos == adj ? free_pos : data_pos, adj, steps + 1);
        }
      }
    }
  }

  throw std::runtime_error("search space exhausted, no answer found");
}

std::istream& operator>>(std::istream& is, Node& node) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (int x{}, y{}; ls >> skip("/dev/grid/node-x"s) >> x >> skip("-y"s) >> y) {
      if (int size{}; ls >> std::ws >> size >> skip("T"s)) {
        if (int used{}; ls >> std::ws >> used >> skip("T"s)) {
          if (int avail{}; ls >> std::ws >> avail >> skip("T"s)) {
            if (size > 0 and used <= size and avail + used == size) {
              node = {.pos = Vec2(x, y), .size = size, .used = used};
              return is;
            }
          }
        }
      }
    }
    throw std::runtime_error(std::format("failed parsing Node from line '{}'", line));
  }
  return is;
}

auto parse_nodes(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (is >> skip("root@ebhq-gridcenter#"s, "df"s, "-h"s)) {
    if (is >> std::ws >> skip("Filesystem"s, "Size"s, "Used"s, "Avail"s, "Use%"s) >> std::ws) {
      if (auto nodes{views::istream<Node>(is) | ranges::to<std::vector>()}; is.eof()) {
        return nodes;
      }
    }
  }
  throw std::runtime_error("input has an unexpected header");
}

int main() {
  const auto nodes{parse_nodes("/dev/stdin")};

  const auto part1{count_viable_pairs(nodes)};
  const auto part2{find_shortest_path(nodes)};

  std::println("{} {}", part1, part2);

  return 0;
}
