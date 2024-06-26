#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Item {
  int min_n{};
  int max_n{};
  char ch{};
  std::string password;
};

std::istream& operator>>(std::istream& is, Item& item) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (int min_n{}, max_n{}; ls >> min_n >> skip("-"s) >> max_n) {
      if (char ch{}; ls >> ch >> skip(":"s)) {
        if (std::string pw; ls >> pw) {
          item = {min_n, max_n, ch, pw};
        }
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

int main() {
  int part1{};
  int part2{};
  for (auto&& [min_n, max_n, ch, password] : aoc::parse_items<Item>("/dev/stdin")) {
    const auto n_required{ranges::count(password, ch)};
    part1 += int{min_n <= n_required and n_required <= max_n};
    part2 += int{(password.at(min_n - 1) == ch) != (password.at(max_n - 1) == ch)};
  }
  std::println("{} {}", part1, part2);
  return 0;
}
