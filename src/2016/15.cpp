#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Disc {
  int pos{};
  int mod{};
};

std::istream& operator>>(std::istream& is, Disc& disc) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (int id{}, mod{}, time{}, pos{}; ls >> skip("Disc #"s) >> id >> std::ws >> skip("has"s)
                                        >> mod >> std::ws >> skip("positions; at time="s) >> time
                                        >> skip(", it is at position"s) >> pos >> skip("."s)) {
      if (id > 0 and time == 0 and (ls >> std::ws).eof()) {
        disc = {pos, mod};
      } else {
        throw std::runtime_error(std::format("failed parsing line '{}'", line));
      }
    }
  }
  return is;
}

auto find_time_to_press(const auto& discs) {
  for (auto t{0UL}; t < 100'000'000; ++t) {
    const auto has_open_slot{[=](auto i, const Disc& d) {
      return ((d.pos + t + i + 1) % d.mod) == 0;
    }};
    if (ranges::all_of(my_std::views::enumerate(discs), my_std::apply_fn(has_open_slot))) {
      return t;
    }
  }
  throw std::runtime_error("search space exhausted, no answer found");
}

int main() {
  auto discs{aoc::parse_items<Disc>("/dev/stdin")};

  const auto part1{find_time_to_press(discs)};

  discs.push_back({0, 11});
  const auto part2{find_time_to_press(discs)};

  std::println("{} {}", part1, part2);

  return 0;
}
