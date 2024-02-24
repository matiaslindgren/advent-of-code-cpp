import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = aoc::Vec2;
using Mark = std::pair<int, int>;

constexpr auto intmax{std::numeric_limits<int>::max()};

struct Step {
  Vec2 dir;
  int len;
};

std::istream& operator>>(std::istream& is, Step& step) {
  if (char ch; is >> ch) {
    Vec2 dir;
    switch (ch) {
      case 'U': {
        dir = {.y = -1};
      } break;
      case 'R': {
        dir = {.x = 1};
      } break;
      case 'D': {
        dir = {.y = 1};
      } break;
      case 'L': {
        dir = {.x = -1};
      } break;
      default: {
        is.setstate(std::ios_base::failbit);
      } break;
    }
    if (int len; is >> len) {
      step = {dir, len};
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Step");
}

void find_paths(const auto& wire, const int mark, auto& paths) {
  Vec2 p{};
  int n_steps{};
  for (auto&& [dir, len] : wire) {
    for (int i{}; i < len; ++i) {
      p += dir;
      n_steps += 1;
      if (not paths.contains(p)) {
        paths[p] = {mark, n_steps};
      } else if (auto&& [prev, n]{paths.at(p)}; prev and prev != mark) {
        paths[p] = {0, n_steps + n};
      }
    }
  }
}

auto search(const auto& wire1, const auto& wire2) {
  std::unordered_map<Vec2, Mark> paths;
  find_paths(wire1, 1, paths);
  find_paths(wire2, 2, paths);
  return my_std::ranges::fold_left(paths, std::pair{intmax, intmax}, [](auto&& res, auto&& kv) {
    auto&& [part1, part2]{res};
    auto&& [pos, mark]{kv};
    auto&& [wire, n_steps]{mark};
    if (wire == 0) {
      return std::pair{
          std::min(part1, pos.distance(Vec2{})),
          std::min(part2, n_steps),
      };
    }
    return res;
  });
}

auto parse_input(const std::string path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, ',', ' ');

  std::istringstream is{input};
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    auto wire1{views::istream<Step>(ls) | ranges::to<std::vector>()};

    if (ls.eof() and std::getline(is, line)) {
      ls = std::istringstream(line);
      auto wire2{views::istream<Step>(ls) | ranges::to<std::vector>()};

      if (ls.eof()) {
        return std::pair{wire1, wire2};
      }
    }
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto [wire1, wire2]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(wire1, wire2)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
