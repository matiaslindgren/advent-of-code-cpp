#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Step {
  std::string src;
  std::string lhs;
  std::string rhs;
};

using Steps = std::unordered_map<std::string, Step>;

constexpr auto count_length(
    const std::string_view loop,
    const Steps& steps,
    const std::string_view start,
    const auto is_end
) {
  long n{};
  std::string current{start};
  for (std::size_t i{}; not is_end(current); i = (i + 1) % loop.size()) {
    const auto& step{steps.at(current)};
    current = (loop[i] == 'L' ? step.lhs : step.rhs);
    ++n;
  }
  return n;
}

auto find_part1(std::string_view loop, const Steps& steps) {
  return count_length(loop, steps, "AAA"s, [](auto s) { return s == "ZZZ"s; });
}

auto find_part2(std::string_view loop, const Steps& steps) {
  auto starts{
      steps | views::keys | views::filter([](const auto& s) { return s.back() == 'A'; })
      | ranges::to<std::vector>()
  };
  return ranges::fold_left(starts, 1L, [&](auto lcm, auto start) {
    auto n{count_length(loop, steps, start, [](auto s) { return s.back() == 'Z'; })};
    return std::lcm(lcm, n);
  });
}

std::istream& operator>>(std::istream& is, Step& step) {
  if (Step s; is >> std::ws >> s.src >> s.lhs >> s.rhs) {
    step = s;
  }
  return is;
}

auto parse_input(std::string_view path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, '=', ' ');
  ranges::replace(input, '(', ' ');
  ranges::replace(input, ',', ' ');
  ranges::replace(input, ')', ' ');
  std::istringstream is{input};
  if (std::string loop; is >> loop and not loop.empty()
                        and ranges::all_of(loop, [](auto ch) { return ch == 'L' or ch == 'R'; })) {
    auto steps{
        views::istream<Step>(is)
        | views::transform([](const auto& step) { return std::pair{step.src, step}; })
        | ranges::to<Steps>()
    };
    if (is.eof()) {
      return std::pair{loop, steps};
    }
    throw std::runtime_error("failed parsing steps");
  }
  throw std::runtime_error("loop must be [LR]+");
}

int main() {
  auto [loop, steps]{parse_input("/dev/stdin")};

  const auto part1{find_part1(loop, steps)};
  const auto part2{find_part2(loop, steps)};

  std::println("{} {}", part1, part2);

  return 0;
}
