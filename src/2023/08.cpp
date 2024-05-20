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

std::istream& operator>>(std::istream& is, Step& step) {
  if (std::string src; is >> src >> skip(" = "s)) {
    if (std::string lhs; is >> skip("("s) >> lhs and lhs.ends_with(","s)) {
      lhs.pop_back();
      if (std::string rhs; is >> rhs and rhs.ends_with(")"s)) {
        rhs.pop_back();
        step = {src, lhs, rhs};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Step");
}

using Steps = std::unordered_map<std::string, Step>;

constexpr auto count_length(
    const std::string_view loop,
    const Steps& steps,
    const std::string_view start,
    const auto is_end
) {
  long n{};
  std::string current{start};
  for (auto i{0uz}; not is_end(current); i = (i + 1) % loop.size()) {
    const auto step{steps.at(current)};
    current = (loop[i] == 'L' ? step.lhs : step.rhs);
    ++n;
  }
  return n;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  std::string loop;
  input >> loop;

  if (not ranges::all_of(loop, [](auto ch) { return ch == 'L' or ch == 'R'; })) {
    throw std::runtime_error("loop must consist of only L or R");
  }

  const auto steps{
      views::istream<Step>(input)
      | views::transform([](const auto& step) { return std::pair{step.src, step}; })
      | ranges::to<Steps>()
  };

  const auto starts{
      steps | views::keys | views::filter([](const auto& s) { return s.back() == 'A'; })
      | ranges::to<std::vector>()
  };

  const auto part1{count_length(loop, steps, "AAA"s, [](auto s) { return s == "ZZZ"s; })};
  const auto part2{ranges::fold_left(starts, 1L, [&](auto lcm, auto start) {
    auto n{count_length(loop, steps, start, [](auto s) { return s.back() == 'Z'; })};
    return std::lcm(lcm, n);
  })};

  std::println("{} {}", part1, part2);

  return 0;
}
