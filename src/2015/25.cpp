#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::skip;

auto parse_input(std::istream& is) {
  if (int row{}, col{};
      is
      >> skip("To continue, please consult the code grid in the manual."s, "Enter the code at row"s)
      >> row >> skip(", column"s) >> col) {
    return std::pair{row, col};
  }
  throw std::runtime_error("failed parsing input");
}

using UInt = unsigned long;

UInt find(const int target_row, const int target_col) {
  const UInt factor{252533U};
  const UInt rem{33554393U};
  for (UInt value{20151125U}, diag{1U};; ++diag) {
    for (UInt y{diag}, x{1U}; y > 0; --y, ++x) {
      if (y == target_row and x == target_col) {
        return value;
      }
      value = (factor * value) % rem;
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto [row, col]{parse_input(input)};
  const auto part1{find(row, col)};
  std::println("{}", part1);

  return 0;
}
