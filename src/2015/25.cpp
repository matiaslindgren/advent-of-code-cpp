import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

std::pair<int, int> parse_input(std::istream& is) {
  using std::operator""s;
  using aoc::skip;
  if (int row, col;
      skip(is, "To continue, please consult the code grid in the manual.  Enter the code at row "s)
      && is >> row && skip(is, ", column "s) && is >> col) {
    return {row, col};
  }
  throw std::runtime_error("failed parsing input");
}

using UInt = unsigned long;

UInt find(const int target_row, const int target_col) {
  const UInt factor{252533u};
  const UInt rem{33554393u};
  for (UInt value{20151125u}, diag{1u};; ++diag) {
    for (UInt y{diag}, x{1u}; y > 0; --y, ++x) {
      if (y == target_row && x == target_col) {
        return value;
      }
      value = (factor * value) % rem;
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto [row, col] = parse_input(input);
  const auto part1{find(row, col)};
  std::print("{}\n", part1);

  return 0;
}
