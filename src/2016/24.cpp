import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Distance {
  int dst, len;
};

struct Grid {
  std::unordered_map<std::size_t, int> targets;
  std::vector<char> tiles;
  std::size_t width{};

  std::vector<Distance> bfs(const auto begin) const;
};

std::istream& operator>>(std::istream& is, Grid& grid) {
  Grid g;
  for (std::string line; std::getline(is, line) && !line.empty();) {
    if (!g.width) {
      g.width = line.size();
    } else if (line.size() != g.width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    std::istringstream ls{line};
    for (char ch; is && ls >> ch;) {
      switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7': {
          g.targets[g.tiles.size()] = ch - '0';
          g.tiles.push_back('.');
        } break;
        case '.':
        case '#': {
          g.tiles.push_back(ch);
        } break;
        default: {
          is.setstate(std::ios_base::failbit);
        } break;
      }
    }
  }
  if (!g.tiles.empty()) {
    grid = g;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Grid");
}

std::vector<Distance> Grid::bfs(const auto begin) const {
  std::vector<Distance> dist2target;
  std::vector<bool> visited(tiles.size(), false);
  for (std::deque q = {std::pair{begin, 0}}; !q.empty(); q.pop_front()) {
    const auto& [i, len] = q.front();
    if (visited[i] || tiles.at(i) == '#') {
      continue;
    }
    visited[i] = true;
    if (targets.contains(i)) {
      dist2target.emplace_back(targets.at(i), len);
    }
    q.emplace_back(i - 1, len + 1);
    q.emplace_back(i + 1, len + 1);
    q.emplace_back(i - width, len + 1);
    q.emplace_back(i + width, len + 1);
  }
  return dist2target;
}

std::pair<int, int> search_min_paths(const Grid& grid) {
  const auto n_targets{grid.targets.size()};
  std::vector<int> dist_data(n_targets * n_targets, std::numeric_limits<int>::max());
  const auto dist{std::mdspan(dist_data.data(), n_targets, n_targets)};

  for (auto [begin, src] : grid.targets) {
    for (const auto [dst, len] : grid.bfs(begin)) {
      dist[src, dst] = dist[dst, src] = len;
    }
  }

  auto part1{std::numeric_limits<int>::max()};
  auto part2{std::numeric_limits<int>::max()};

  {
    auto targets{grid.targets | views::values | ranges::to<std::vector<int>>()};
    do {
      const auto begin{targets.front()};
      const auto path{my_std::ranges::fold_left(
          targets | views::drop(1),
          Distance{begin, 0},
          [&dist](const auto prev, const auto dst) {
            const auto [src, len] = prev;
            return Distance{dst, len + dist[src, dst]};
          }
      )};
      part1 = std::min(part1, path.len);
      part2 = std::min(part2, path.len + dist[begin, path.dst]);
    } while (std::ranges::next_permutation(targets).found);
  }

  return {part1, part2};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Grid grid;
  input >> grid;

  const auto [part1, part2] = search_min_paths(grid);
  std::print("{} {}\n", part1, part2);

  return 0;
}
