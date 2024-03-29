import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Unit {
  int type{};
  bool polarity{false};
  bool alive{true};

  bool destroys(const Unit& rhs) const {
    return type == rhs.type and polarity != rhs.polarity;
  }
};

std::istream& operator>>(std::istream& is, Unit& unit) {
  if (char ch; is >> ch) {
    if ('a' <= ch and ch <= 'z') {
      unit = {ch - 'a', false};
    } else if ('A' <= ch and ch <= 'Z') {
      unit = {ch - 'A', true};
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Unit");
}

std::size_t react(auto polymer) {
  for (auto alive{ranges::subrange(polymer)};;) {
    // TODO std adjacent
    for (const auto [lhs, rhs] : views::zip(alive, views::drop(alive, 1))) {
      if (lhs.alive and rhs.alive and lhs.destroys(rhs)) {
        lhs.alive = rhs.alive = false;
      }
    }
    if (const auto dead{ranges::remove_if(alive, std::not_fn(&Unit::alive))}; dead.empty()) {
      return alive.size();
    } else {
      alive = ranges::subrange(alive.begin(), dead.begin());
    }
  }
}

auto find_part2(const auto& polymer) {
  return my_std::ranges::fold_left(
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

int main() {
  const auto polymer{aoc::slurp<Unit>("/dev/stdin")};

  const auto part1{react(polymer)};
  const auto part2{find_part2(polymer)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
