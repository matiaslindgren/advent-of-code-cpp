import std;

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

bool is_forbidden(char ch1, char ch2) {
  switch (ch1) {
    case 'a':
    case 'c':
    case 'p':
    case 'x':
      return ch1 + 1 == ch2;
  }
  return false;
}

auto contains_forbidden_pair(const std::string& s) {
  // TODO(llvm18)
  // return std::any_of(std::views::pairwise(s), is_forbidden); // unpack?
  for (auto it = std::ranges::next(s.begin(), 1, s.end()); it != s.end();
       ++it) {
    if (is_forbidden(*std::prev(it), *it)) {
      return true;
    }
  }
  return false;
}

bool is_nice_part1(const std::string& s) {
  const auto has_3_vowels = std::ranges::count_if(s, is_vowel) >= 3;
  const auto has_letter_pair = std::ranges::adjacent_find(s) != s.end();
  return has_3_vowels && has_letter_pair && !contains_forbidden_pair(s);
}

auto contains_sandwich_letter(const std::string& s) {
  // TODO(llvm18)
  // for (const auto [ch0, ch1, ch2] : std::views::adjacent<3>(s)) {
  for (auto it = std::ranges::next(s.begin(), 2, s.end()); it != s.end();
       ++it) {
    if (*std::prev(it, 2) == *it) {
      return true;
    }
  }
  return false;
}

auto contains_letter_pair_twice(const std::string& s) {
  constexpr auto alphabet_size = ('z' - 'a') + 1;
  constexpr auto n{alphabet_size * alphabet_size};
  std::array<int, n> pair_counts = {0};
  const auto pair2index = [&alphabet_size](auto a, auto b) {
    return (a - 'a') * alphabet_size + (b - 'a');
  };
  char ch0{0};
  // TODO(llvm18)
  // for (const auto [ch1, ch2] : std::views::pairwise(s)) {
  for (auto it = std::ranges::next(s.begin(), 1, s.end()); it != s.end();
       ++it) {
    const auto ch1 = *std::prev(it);
    const auto ch2 = *it;
    if (ch0 == ch1 && ch1 == ch2) {
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
  return contains_sandwich_letter(s) && contains_letter_pair_twice(s);
}

int main() {
  const auto lines = std::views::istream<std::string>(std::cin) |
                     std::ranges::to<std::vector<std::string>>();

  const auto part1{std::ranges::count_if(lines, is_nice_part1)};
  const auto part2{std::ranges::count_if(lines, is_nice_part2)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
