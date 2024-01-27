import std;
import aoc;
import my_std;

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Direction : char {
  right = 'R',
  down = 'D',
  left = 'L',
  up = 'U',
};

std::istream& operator>>(std::istream& is, Direction& dir) {
  if (std::underlying_type_t<Direction> ch; is >> ch) {
    if ('0' <= ch && ch <= '3') {
      ch = "RDLU"s[ch - '0'];
    }
    switch (ch) {
      case std::to_underlying(Direction::right):
      case std::to_underlying(Direction::down):
      case std::to_underlying(Direction::left):
      case std::to_underlying(Direction::up):
        dir = {ch};
        return is;
      default:
        is.setstate(std::ios_base::failbit);
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Direction");
}

struct Step {
  Direction dir;
  int len;
};
using Steps = std::pair<Step, Step>;

std::istream& operator>>(std::istream& is, Steps& steps) {
  using aoc::skip;
  if (auto [dir1, len1, colour] = std::tuple{Direction{}, int{}, ""s};
      is >> dir1 >> len1 >> colour) {
    std::stringstream ls{colour};
    int len2{0};
    if (skip(ls, "(#"s)) {
      for (char i{}, ch; i < 5 && ls >> ch; ++i) {
        len2 = 16 * len2 + std::stoi(""s + ch, nullptr, 16);
      }
      if (Direction dir2; ls >> dir2 && skip(ls, ")"s)) {
        steps = {{dir1, len1}, {dir2, len2}};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Steps");
}

struct Point2D {
  using Int = long;
  Int x;
  Int y;
  constexpr Point2D operator+(const Point2D& rhs) const {
    return {x + rhs.x, y + rhs.y};
  }
};

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0L, std::plus{})};

// TODO ranges::adjacent
constexpr decltype(auto) window2(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto dig(ranges::view auto&& steps) {
  std::vector trench{{Point2D{}}};
  auto trench_len{0uz};
  for (const auto& step : steps) {
    Point2D delta{};
    switch (step.dir) {
      case Direction::right: {
        delta.x += step.len;
      } break;
      case Direction::down: {
        delta.y += step.len;
      } break;
      case Direction::left: {
        delta.x -= step.len;
      } break;
      case Direction::up: {
        delta.y -= step.len;
      } break;
    }
    trench.push_back(trench.back() + delta);
    trench_len += step.len;
  }
  auto&& trapezoid_areas{views::transform(window2(trench), [](auto p1p2) {
    const auto& [p1, p2] = p1p2;
    return (p1.y + p2.y) * (p1.x - p2.x);
  })};
  const auto trench_area{std::abs(sum(trapezoid_areas)) / 2};
  return trench_len / 2 + trench_area + 1;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto steps{views::istream<Steps>(input) | ranges::to<std::vector>()};

  const auto part1{dig(views::elements<0>(steps))};
  const auto part2{dig(views::elements<1>(steps))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
