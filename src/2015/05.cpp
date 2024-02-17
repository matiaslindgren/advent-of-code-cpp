import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr static auto alphabet_size = ('z' - 'a') + 1;

constexpr auto pair2index(char ch0, char ch1) {
  return (ch0 - 'a') * alphabet_size + (ch1 - 'a');
}

bool is_vowel(char ch) {
  switch (ch) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
      return true;
  }
  return false;
}

bool is_forbidden_pair(char ch0, char ch1) {
  switch (ch0) {
    case 'a':
    case 'c':
    case 'p':
    case 'x':
      return ch0 + 1 == ch1;
  }
  return false;
}

bool is_nice_part1(const std::string& s) {
  const auto has_3_vowels{ranges::count_if(s, is_vowel) >= 3};
  const auto has_letter_pair{ranges::adjacent_find(s) != s.end()};
  const auto has_forbidden_pair{
      ranges::any_of(views::zip(s, views::drop(s, 1)), my_std::apply_fn(is_forbidden_pair))
  };
  return has_3_vowels and has_letter_pair and not has_forbidden_pair;
}

auto contains_sandwich_letter(const std::string& s) {
  return ranges::any_of(views::zip(s, views::drop(s, 2)), my_std::apply_fn(std::equal_to{}));
}

auto contains_letter_pair_twice(const std::string& s) {
  constexpr static auto n{alphabet_size * alphabet_size};
  std::array<int, n> pair_counts;
  pair_counts.fill(0);
  char ch0{0};
  // TODO(llvm18)
  // for (const auto [ch1, ch2] : views::pairwise(s)) {
  for (const auto [ch1, ch2] : views::zip(s, views::drop(s, 1))) {
    if (ch0 == ch1 and ch1 == ch2) {
      ch0 = 0;
      continue;
    } else {
      ch0 = ch1;
    }
    if (++pair_counts[pair2index(ch1, ch2)] == 2) {
      return true;
    }
  }
  return false;
}

bool is_nice_part2(const std::string& s) {
  return contains_sandwich_letter(s) and contains_letter_pair_twice(s);
}

int main() {
  std::ios::sync_with_stdio(false);
  const auto lines{views::istream<std::string>(std::cin) | ranges::to<std::vector>()};

  const auto part1{ranges::count_if(lines, is_nice_part1)};
  const auto part2{ranges::count_if(lines, is_nice_part2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
