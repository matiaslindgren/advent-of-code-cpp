import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

inline constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto parse_input(const std::string path) {
  std::istringstream input{aoc::slurp_file(path)};
  auto masses{views::istream<int>(input) | ranges::to<std::vector>()};
  if (input.eof()) {
    return masses;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  auto masses{parse_input("/dev/stdin")};

  std::vector<int> results;

  for (; ranges::any_of(masses, [](int x) { return x > 0; });) {
    ranges::for_each(masses, [](int& x) { x = std::max(0, x / 3 - 2); });
    results.push_back(sum(masses));
  }

  const auto part1{results.front()};
  const auto part2{sum(results)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
