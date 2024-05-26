#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<long>;

struct Step {
  Vec2 delta;

  Step() = default;
  explicit Step(char direction, int length) {
    switch (direction) {
      case '0':
      case 'R': {
        delta = Vec2(length, 0);
      } break;
      case '1':
      case 'D': {
        delta = Vec2(0, length);
      } break;
      case '2':
      case 'L': {
        delta = Vec2(-length, 0);
      } break;
      case '3':
      case 'U': {
        delta = Vec2(0, -length);
      } break;
      default:
        throw std::runtime_error(std::format("unknown direction '{}'", direction));
    }
  }
};

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

// TODO (llvm19?) ranges::adjacent
decltype(auto) window2(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

auto trapezoid_area(const Vec2& p1, const Vec2& p2) {
  return (p1.y() + p2.y()) * (p1.x() - p2.x());
}

auto dig(ranges::view auto&& steps) {
  std::vector trench{Vec2()};
  long trench_len{};
  for (const Step& s : steps) {
    trench.push_back(trench.back() + s.delta);
    trench_len += s.delta.abs().sum();
  }
  const auto areas{views::transform(window2(trench), my_std::apply_fn(trapezoid_area))};
  const auto trench_area{std::abs(sum(areas)) / 2};
  return trench_len / 2 + trench_area + 1;
}

using StepsPair = std::pair<Step, Step>;

std::istream& operator>>(std::istream& is, StepsPair& sp) {
  if (auto [dir1, len1, colour]{std::tuple{char{}, int{}, ""s}};
      is >> std::ws >> dir1 >> len1 >> colour) {
    std::istringstream ls{colour};
    int len2{};
    if (ls >> skip("(#"s)) {
      for (char i{}, ch{}; i < 5 and ls >> ch; ++i) {
        len2 = 16 * len2 + std::stoi(""s + ch, nullptr, 16);
      }
      if (char dir2{}; ls >> dir2 >> skip(")"s) >> std::ws) {
        sp = {Step(dir1, len1), Step(dir2, len2)};
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error("failed parsing StepsPair");
    }
  }
  return is;
}

int main() {
  const auto step_pairs{aoc::parse_items<StepsPair>("/dev/stdin")};

  const auto part1{dig(views::elements<0>(step_pairs))};
  const auto part2{dig(views::elements<1>(step_pairs))};

  std::println("{} {}", part1, part2);

  return 0;
}
