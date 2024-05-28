#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Unit {
  int type{};
  bool polarity{false};
  bool alive{true};

  [[nodiscard]]
  bool destroys(const Unit& rhs) const {
    return type == rhs.type and polarity != rhs.polarity;
  }
};

std::size_t react(auto polymer) {
  for (auto alive{ranges::subrange(polymer)};;) {
    // TODO (llvm19?) std adjacent
    for (auto&& [lhs, rhs] : views::zip(alive, views::drop(alive, 1))) {
      if (lhs.alive and rhs.alive and lhs.destroys(rhs)) {
        lhs.alive = rhs.alive = false;
      }
    }
    const auto dead{ranges::remove_if(alive, std::not_fn(&Unit::alive))};
    if (dead.empty()) {
      return alive.size();
    }
    alive = ranges::subrange(alive.begin(), dead.begin());
  }
}

auto find_part2(const auto& polymer) {
  return ranges::fold_left(
      views::iota(0, 'z' - 'a' + 1),
      polymer.size(),
      [&polymer](const auto best, const auto without) {
        const auto is_included{[&without](const auto& u) { return u.type != without; }};
        return std::min(
            best,
            react(polymer | views::filter(is_included) | ranges::to<std::vector>())
        );
      }
  );
}

std::istream& operator>>(std::istream& is, Unit& unit) {
  if (char ch{}; is >> ch) {
    if ('a' <= ch and ch <= 'z') {
      unit = {ch - 'a', false};
    } else if ('A' <= ch and ch <= 'Z') {
      unit = {ch - 'A', true};
    } else {
      throw std::runtime_error("invalid input, every char must be in [a-zA-Z]");
    }
  }
  return is;
}

int main() {
  const auto polymer{aoc::parse_items<Unit>("/dev/stdin")};

  const auto part1{react(polymer)};
  const auto part2{find_part2(polymer)};

  std::println("{} {}", part1, part2);

  return 0;
}
