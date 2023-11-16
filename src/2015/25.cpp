import std;

namespace ranges = std::ranges;
namespace views = std::views;

std::pair<int, int> parse_input(std::istream& is) {
  const auto skip = [&is](std::string_view s) {
    auto it{s.begin()};
    for (char ch; it != s.end() && is.get(ch) && *it == ch; ++it) {
    }
    return it == s.end();
  };
  if (int row, col;
      skip("To continue, please consult the code grid in the manual.  Enter the code at row ")
      && is >> row && skip(", column ") && is >> col) {
    return {row, col};
  }
  throw std::runtime_error("failed parsing input");
}

using ull = unsigned long long;

ull find(const int target_row, const int target_col) {
  const ull factor{252533};
  const ull rem{33554393};
  ull value{20151125};
  for (ull diag{1};; ++diag) {
    for (ull y{diag}, x{1}; y > 0; --y, ++x) {
      if (y == target_row && x == target_col) {
        return value;
      }
      value = (factor * value) % rem;
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto [row, col] = parse_input(std::cin);
  const auto part1{find(row, col)};
  std::print("{}\n", part1);

  return 0;
}
