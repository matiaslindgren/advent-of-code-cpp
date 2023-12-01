import std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr int char2num(const auto ch) {
  return ch - '0';
}

template <std::input_or_output_iterator It>
  requires std::random_access_iterator<It>
const int find_str_digit(const It begin, const It end) {
  static const std::unordered_map<std::string, int> str2num{
      {{"one", 1},
       {"two", 2},
       {"three", 3},
       {"four", 4},
       {"five", 5},
       {"six", 6},
       {"seven", 7},
       {"eight", 8},
       {"nine", 9}}
  };
  for (auto n{3uz}; n <= 5uz; ++n) {
    const std::string s{begin, ranges::next(begin, n, end)};
    if (const auto num{str2num.find(s)}; num != str2num.end()) {
      return num->second;
    }
  }
  return 0;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto isdigit{[](unsigned char ch) { return std::isdigit(ch); }};

  int part1{}, part2{};

  for (std::string line; std::getline(std::cin, line);) {
    const auto lhs1{ranges::find_if(line, isdigit)};
    if (lhs1 == line.end()) {
      continue;
    }
    const auto rhs1{ranges::find_if(views::reverse(line), isdigit).base() - 1};
    const auto lhs_val1{char2num(*lhs1) * 10};
    const auto rhs_val1{char2num(*rhs1)};
    part1 += lhs_val1 + rhs_val1;

    int lhs_val2{};
    for (auto lhs2{line.begin()}; lhs2 < lhs1 && !lhs_val2; ++lhs2) {
      lhs_val2 = find_str_digit(lhs2, lhs1) * 10;
    }
    int rhs_val2{};
    for (auto rhs2{ranges::prev(line.end(), 1, rhs1)}; rhs1 < rhs2 && !rhs_val2; --rhs2) {
      rhs_val2 = find_str_digit(rhs2, line.end());
    }
    part2 += (lhs_val2 ? lhs_val2 : lhs_val1) + (rhs_val2 ? rhs_val2 : rhs_val1);
  }

  std::print("{} {}\n", part1, part2);

  return 0;
}
