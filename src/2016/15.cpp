import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Disc {
  int pos;
  int mod;
};

std::istream& operator>>(std::istream& is, Disc& disc) {
  using aoc::skip;
  using std::operator""s;
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    // clang-format off
    if (int id, mod, time, pos;
        skip(ls, "Disc #"s) && ls >> id && id > 0
        && skip(ls, " has"s) && ls >> mod && skip(ls, " positions;"s)
        && skip(ls, " at time="s) && ls >> time && time == 0
        && skip(ls, ", it is at position"s) && ls >> pos && skip(ls, "."s)) {
      disc = {pos, mod};
      return is;
    }
    // clang-format on
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Disc");
}

auto find_time_to_press(const auto& discs) {
  // TODO no loop, find common multiple/divisor
  for (auto t{0uz}; t < 100'000'000; ++t) {
    const auto has_open_slot{[=](const auto& p) {
      const auto& [i, disc] = p;
      return ((disc.pos + t + i + 1) % disc.mod) == 0;
    }};
    if (ranges::all_of(my_std::views::enumerate(discs), has_open_slot)) {
      return t;
    }
  }
  return std::numeric_limits<std::size_t>::max();
}

int main() {
  std::ios_base::sync_with_stdio(false);

  auto discs{views::istream<Disc>(std::cin) | ranges::to<std::vector>()};

  const auto part1{find_time_to_press(discs)};

  discs.push_back({0, 11});
  const auto part2{find_time_to_press(discs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
