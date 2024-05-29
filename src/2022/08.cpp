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
    part1 += int{any_visible};
    part2 = std::max(part2, scenic_score);
  }
  return std::pair{part1, part2};
}

Trees parse_trees(std::string_view path) {
  Trees trees;
  for (Vec2 p; const std::string& line : aoc::slurp_lines(path)) {
    for (p.x() = 0; char ch : line) {
      if (not aoc::is_digit(ch)) {
        throw std::runtime_error("all non-whitespace input must be digits");
      }
      trees[p] = ch - '0';
      p.x() += 1;
    }
    p.y() += 1;
  }
  return trees;
}

int main() {
  const auto [part1, part2]{search(parse_trees("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
