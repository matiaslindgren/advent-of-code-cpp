#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct History {
  std::vector<int> values;
};

auto adjacent_diff(const auto& hist) {
  return views::zip(views::drop(hist, 1), hist) | views::transform(my_std::apply_fn(std::minus{}))
         | ranges::to<std::vector>();
}

auto search(const auto& histories) {
  long part1{};
  long part2{};
  for (const History& h : histories) {
    std::vector<int> front;
    std::vector<int> back;
    for (auto diff{h.values}; ranges::any_of(diff, std::identity{}); diff = adjacent_diff(diff)) {
      front.push_back(diff.front());
      back.push_back(diff.back());
    }
    part1 += ranges::fold_left(back, 0L, std::plus{});
    part2 += my_std::ranges::fold_right(front, 0L, std::minus{});
  }
  return std::pair{part1, part2};
}

std::istream& operator>>(std::istream& is, History& h) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    h.values = views::istream<int>(ls) | ranges::to<std::vector>();
  }
  return is;
}

int main() {
  const auto [part1, part2]{search(aoc::parse_items<History>("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
