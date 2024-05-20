#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Present {
  int l;
  int w;
  int h;
  long surface_area() const {
    return 2 * (l * w + w * h + h * l);
  }
  long slack_size() const {
    return std::min(std::min(l * w, w * h), h * l);
  }
  long bow_size() const {
    return 2 * std::min(std::min(l + w, w + h), h + l);
  }
  long ribbon_size() const {
    return l * w * h;
  }
};

std::istream& operator>>(std::istream& is, Present& p) {
  char ch;
  int l, w, h;
  if (is >> l >> ch and ch == 'x' and is >> w >> ch and ch == 'x' and is >> h) {
    p = {l, w, h};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Present");
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

int main() {
  const auto presents{aoc::parse_items<Present>("/dev/stdin")};

  const auto part1{sum(views::transform(presents, [](const auto& p) {
    return p.surface_area() + p.slack_size();
  }))};
  const auto part2{
      sum(views::transform(presents, [](const auto& p) { return p.ribbon_size() + p.bow_size(); }))
  };

  std::println("{} {}", part1, part2);

  return 0;
}
