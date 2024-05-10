#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using Vec2 = ndvec::vec2<int>;
using Trees = std::unordered_map<Vec2, int>;

auto search(const Trees& trees) {
  int part1{};
  int part2{};
  for (auto [center, center_height] : trees) {
    bool any_visible{false};
    int scenic_score{1};
    for (Vec2 direction : Vec2(0, 0).adjacent()) {
      int score{};
      bool visible{true};
      for (Vec2 t{center + direction}; trees.contains(t); t += direction) {
        ++score;
        if (trees.at(t) >= center_height) {
          visible = false;
          break;
        }
      }
      any_visible |= visible;
      scenic_score *= score;
    }
    part1 += any_visible;
    part2 = std::max(part2, scenic_score);
  }
  return std::pair{part1, part2};
}

Trees parse_trees(std::string_view path) {
  Trees trees;
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string row; is >> row; p.y() += 1) {
    p.x() = 0;
    for (char ch : row) {
      switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          trees[p] = ch - '0';
          p.x() += 1;
          continue;
      }
      throw std::runtime_error("all non-whitespace input must be digits");
    }
  }
  if (trees.empty() or (is.fail() and not is.eof())) {
    throw std::runtime_error("unknown error while parsing trees");
  }
  return trees;
}

int main() {
  const auto [part1, part2]{search(parse_trees("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
