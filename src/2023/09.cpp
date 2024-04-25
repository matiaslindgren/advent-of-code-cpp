#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

std::vector<Ints> parse_input(std::istream& is) {
  std::vector<Ints> histories;
  for (std::string line; std::getline(is, line);) {
    std::istringstream ls{line};
    histories.push_back(views::istream<int>(ls) | ranges::to<Ints>());
  }
  if (is.eof()) {
    return histories;
  }
  throw std::runtime_error("failed parsing Ints");
}

Ints adjacent_diff(const auto& hist) {
  return views::zip(views::drop(hist, 1), hist) | views::transform(my_std::apply_fn(std::minus{}))
         | ranges::to<Ints>();
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto histories{parse_input(input)};

  long part1{0}, part2{0};
  for (const auto& history : histories) {
    Ints front{}, back{};
    for (Ints diff{history}; ranges::any_of(diff, std::identity{}); diff = adjacent_diff(diff)) {
      front.push_back(diff.front());
      back.push_back(diff.back());
    }
    part1 += ranges::fold_left(back, 0L, std::plus{});
    part2 += my_std::ranges::fold_right(front, 0L, std::minus{});
  }

  std::print("{} {}\n", part1, part2);

  return 0;
}
