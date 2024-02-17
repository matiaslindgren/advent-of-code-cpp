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
        skip(ls, "Disc #"s) and ls >> id and id > 0
        and skip(ls, " has"s) and ls >> mod and skip(ls, " positions;"s)
        and skip(ls, " at time="s) and ls >> time and time == 0
        and skip(ls, ", it is at position"s) and ls >> pos and skip(ls, "."s)) {
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
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  auto discs{views::istream<Disc>(input) | ranges::to<std::vector>()};

  const auto part1{find_time_to_press(discs)};

  discs.push_back({0, 11});
  const auto part2{find_time_to_press(discs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
