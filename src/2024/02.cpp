#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Report = std::vector<int>;

constexpr auto is_safe{[](auto&& report) {
  // TODO(llvm20) pairwise
  auto diffs{views::transform(views::zip(report, views::drop(report, 1)), [](auto&& pair) {
    auto [lhs, rhs]{pair};
    return lhs - rhs;
  })};
  return (
      ranges::all_of(diffs, [](int d) { return 1 <= d and d <= 3; })
      || ranges::all_of(diffs, [](int d) { return -3 <= d and d <= -1; })
  );
}};

auto count_safe(auto reports) {
  int part1{};
  int part2{};
  for (const Report& report : reports) {
    if (is_safe(report)) {
      part1 += 1;
      part2 += 1;
    } else {
      for (size_t i_drop{}; i_drop < report.size(); ++i_drop) {
        // TODO(c++26 llvm20) views::concat
        Report report2;
        report2.append_range(views::take(report, i_drop));
        report2.append_range(views::drop(report, i_drop + 1));
        if (is_safe(report2)) {
          part2 += 1;
          break;
        }
      }
    }
  }
  return std::pair{part1, part2};
}

std::vector<Report> parse_reports(std::string path) {
  const auto line_to_ints{[](const std::string& line) {
    std::istringstream ls{line};
    return views::istream<int>(ls) | ranges::to<std::vector>();
  }};
  return aoc::slurp_lines(path) | views::transform(line_to_ints) | ranges::to<std::vector>();
}

int main() {
  const auto reports{parse_reports("/dev/stdin")};
  const auto [part1, part2]{count_safe(reports)};
  std::println("{} {}", part1, part2);
  return 0;
}
