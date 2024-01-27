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
    return type == rhs.type && polarity != rhs.polarity;
  }
};

std::istream& operator>>(std::istream& is, Unit& unit) {
  if (char ch; is >> ch) {
    if ('a' <= ch && ch <= 'z') {
      unit = {ch - 'a', false};
    } else if ('A' <= ch && ch <= 'Z') {
      unit = {ch - 'A', true};
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Unit");
}

constexpr auto is_alive{[](const Unit& u) { return u.alive; }};
constexpr auto is_dead{std::not_fn(is_alive)};

std::size_t react(auto polymer) {
  for (auto begin{polymer.begin()}, end{polymer.end()};;) {
    for (auto lhs{begin}; lhs != end; ++lhs) {
      if (const auto rhs{lhs + 1}; rhs != end && lhs->alive && lhs->destroys(*rhs)) {
        lhs->alive = rhs->alive = false;
      }
    }
    if (const auto dead{ranges::remove_if(begin, end, is_dead)}; dead.empty()) {
      return end - begin;
    } else {
      end = dead.begin();
    }
  }
}

auto find_part2(const auto& polymer) {
  return my_std::ranges::fold_left(
      views::iota(0, 'z' - 'a' + 1),
      polymer.size(),
      [&polymer](const auto best, const auto without) {
        return std::min(
            best,
            react(
                views::filter(polymer, [&without](const auto& u) { return u.type != without; })
                | ranges::to<std::vector>()
            )
        );
      }
  );
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto polymer{views::istream<Unit>(input) | ranges::to<std::vector>()};

  const auto part1{react(polymer)};
  const auto part2{find_part2(polymer)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
