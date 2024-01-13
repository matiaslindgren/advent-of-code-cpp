import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using UInt = uint32_t;
static_assert(std::numeric_limits<UInt>::max() == 4294967295);

struct Range {
  UInt begin;
  UInt end;
};

std::istream& operator>>(std::istream& is, Range& range) {
  using aoc::skip;
  using std::operator""s;
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    if (UInt begin, end; ls >> begin && skip(ls, "-"s) && ls >> end && begin < end) {
      range = {begin, end};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Range");
}

auto find_allowed(const auto& excluded) {
  std::vector<Range> allowed = {Range{0u, std::numeric_limits<UInt>::max()}};
  for (const Range& exclude : excluded) {
    for (const Range& allow : std::exchange(allowed, {})) {
      if (allow.end < exclude.begin || exclude.end < allow.begin) {
        allowed.push_back(allow);
      }
      if (allow.begin < exclude.begin && exclude.begin <= allow.end) {
        allowed.emplace_back(allow.begin, exclude.begin - 1);
      }
      if (allow.begin < exclude.end && exclude.end < allow.end) {
        allowed.emplace_back(exclude.end + 1, allow.end);
      }
    }
  }
  return allowed;
}

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto excluded_ranges{views::istream<Range>(input) | ranges::to<std::vector>()};

  const auto allowed{find_allowed(excluded_ranges)};
  if (allowed.empty()) {
    throw std::runtime_error("entire range is excluded");
  }

  const auto part1{ranges::min_element(allowed, {}, [](const auto& r) { return r.begin; })->begin};
  const auto part2{sum(views::transform(allowed, [](const auto& r) { return r.end - r.begin + 1; }))
  };

  std::print("{} {}\n", part1, part2);

  return 0;
}
