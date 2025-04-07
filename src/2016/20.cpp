#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

using UInt = uint32_t;
constexpr auto uint_max{std::numeric_limits<UInt>::max()};
static_assert(uint_max == 4294967295);

struct Range {
  UInt begin;
  UInt end;
};

auto find_allowed(const auto& excluded) {
  std::vector allowed{Range{0U, uint_max}};
  for (const Range& exclude : excluded) {
    for (const Range& allow : std::exchange(allowed, {})) {
      if (allow.end < exclude.begin or exclude.end < allow.begin) {
        allowed.push_back(allow);
      }
      if (allow.begin < exclude.begin and exclude.begin <= allow.end) {
        allowed.emplace_back(allow.begin, exclude.begin - 1);
      }
      if (allow.begin < exclude.end and exclude.end < allow.end) {
        allowed.emplace_back(exclude.end + 1, allow.end);
      }
    }
  }
  return allowed;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

std::istream& operator>>(std::istream& is, Range& range) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (UInt begin{}, end{}; ls >> begin >> skip("-"s) >> end and begin < end) {
      range = {begin, end};
    } else {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

int main() {
  const auto excluded_ranges{aoc::parse_items<Range>("/dev/stdin")};

  const auto allowed{find_allowed(excluded_ranges)};
  if (allowed.empty()) {
    throw std::runtime_error("entire range is excluded");
  }

  const auto part1{ranges::min(views::transform(allowed, &Range::begin))};
  const auto part2{
      sum(views::transform(allowed, [](const auto& r) { return r.end - r.begin + 1; }))
  };

  std::println("{} {}", part1, part2);

  return 0;
}
