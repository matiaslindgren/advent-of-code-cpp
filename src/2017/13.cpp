import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Scanner {
  int depth{}, range{};

  bool is_at_top(const auto t) const {
    const auto n{2 * (range - 1)};
    const auto layer{std::abs(t - n * static_cast<int>(std::round(t / n)))};
    return layer == 0;
  }

  auto severity() const {
    return depth * range;
  }
};

std::istream& operator>>(std::istream& is, Scanner& scanner) {
  using aoc::skip;
  using std::operator""s;
  if (int depth, range; is >> depth && depth >= 0 && skip(is, ":"s) && is >> range && range > 1) {
    scanner = {depth, range};
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Scanner");
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& scanners) {
  return sum(views::transform(scanners, [](const auto& s) {
    return s.is_at_top(s.depth) * s.severity();
  }));
}

auto find_part2(const auto& scanners) {
  for (int t{};; ++t) {
    if (!ranges::any_of(scanners, [&t](const auto& s) { return s.is_at_top(t + s.depth); })) {
      return t;
    }
  }
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto scanners{views::istream<Scanner>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(scanners)};
  const auto part2{find_part2(scanners)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
