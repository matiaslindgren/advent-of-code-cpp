import std;
import aoc;
import my_std;

using std::operator""s;
using aoc::skip;

namespace ranges = std::ranges;
namespace views = std::views;

struct Node {
  int x, y, size, used;
  auto operator<=>(const Node&) const = default;
};

std::istream& operator>>(std::istream& is, Node& node) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (int x, y; skip(ls, "/dev/grid/node-x"s) && ls >> x && skip(ls, "-y"s) && ls >> y) {
      if (int size; ls >> std::ws >> size && skip(ls, "T"s)) {
        if (int used; ls >> std::ws >> used && skip(ls, "T"s)) {
          if (int avail; ls >> std::ws >> avail && skip(ls, "T"s)) {
            if (size > 0 && used <= size && avail + used == size) {
              node = {.x = x, .y = y, .size = size, .used = used};
              return is;
            }
          }
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Node");
}

auto parse_nodes(std::istream& is) {
  if (skip(is, "root@ebhq-gridcenter#"s, "df"s, "-h"s) && is >> std::ws) {
    if (skip(is, "Filesystem"s, "Size"s, "Used"s, "Avail"s, "Use%"s) && is >> std::ws) {
      return views::istream<Node>(is) | ranges::to<std::vector>();
    }
  }
  throw std::runtime_error("input has an unexpected header");
}

auto count_viable_pairs(const auto& nodes) {
  int n_viable{};
  for (const auto& na : nodes) {
    for (const auto& nb : nodes) {
      n_viable += (na != nb) && (na.used > 0) && (na.used + nb.used <= nb.size);
    }
  }
  return n_viable;
}

auto find_shortest_path(const auto& nodes) {
  const auto x_max{ranges::max_element(nodes, {}, &Node::x)->x};
  const auto y_max{ranges::max_element(nodes, {}, &Node::y)->y};

  const auto width{3u + x_max};
  const auto height{3u + y_max};
  const auto grid_size{width * height};

  std::vector<int> used(grid_size, 0), size(grid_size, 0);
  unsigned free_node{};
  for (const auto& n : nodes) {
    const auto index{(n.y + 1) * width + (n.x + 1)};
    if (n.used == 0) {
      free_node = index;
    }
    used[index] = n.used;
    size[index] = n.size;
  }

  unsigned target{width + 1};
  unsigned data_begin{2 * width - 2};

  std::vector<bool> visited(grid_size * grid_size, false);

  for (std::deque q{std::tuple{data_begin, free_node, 0u}}; !q.empty(); q.pop_front()) {
    const auto& [data, f, steps] = q.front();
    if (data == target) {
      return steps;
    }
    if (visited.at(data * grid_size + f)) {
      continue;
    }
    visited[data * grid_size + f] = true;
    const std::array adjacent{f + 1, f + width, f - 1, f - width};
    for (const auto adj : adjacent) {
      if (!size[adj]) {
        continue;
      }
      if (used[adj] <= size[f]) {
        q.emplace_back(data == adj ? f : data, adj, steps + 1);
      }
    }
  }

  throw std::runtime_error("search space exhausted, no answer found");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto nodes{parse_nodes(input)};

  const auto part1{count_viable_pairs(nodes)};
  const auto part2{find_shortest_path(nodes)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
