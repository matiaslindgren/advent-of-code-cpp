import std;

// TODO(llvm17)
namespace my_ranges {
template <class Container>
  requires(!std::ranges::view<Container>)
constexpr auto to() {
  return std::__range_adaptor_closure_t([]<std::ranges::input_range R>(R&& r) {
    Container c;
    std::ranges::copy(r, std::back_inserter(c));
    return c;
  });
}
}  // namespace my_ranges

auto is_vowel(char ch) {
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

auto is_forbidden(char ch1, char ch2) {
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
  // TODO(llvm17)
  // return std::any_of(std::views::pairwise(s), is_forbidden); // unpack?
  auto it1 = s.begin();
  auto it2 = std::next(it1, it1 != s.end());
  for (; it2 != s.end(); ++it1, ++it2) {
    if (is_forbidden(*it1, *it2)) {
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
  // TODO(llvm17)
  // for (const auto [ch0, ch1, ch2] : std::views::adjacent<3>(s)) {
  auto it1 = s.begin();
  auto it2 = std::next(it1, it1 != s.end());
  auto it3 = std::next(it2, it2 != s.end());
  for (; it3 != s.end(); ++it1, ++it2, ++it3) {
    if (*it1 == *it3) {
      return true;
    }
  }
  return false;
}

auto contains_letter_pair_twice(const std::string& s) {
  const auto alphabet_size = ('z' - 'a') + 1;
  std::vector<int> pair_counts(alphabet_size * alphabet_size);
  {
    const auto pair2index = [&alphabet_size](auto a, auto b) {
      return (a - 'a') * alphabet_size + (b - 'a');
    };
    char ch0{0};
    // TODO(llvm17)
    // for (const auto [ch1, ch2] : std::views::pairwise(s)) {
    auto it1 = s.begin();
    auto it2 = std::next(it1, it1 != s.end());
    for (; it2 != s.end(); ++it1, ++it2) {
      const auto ch1 = *it1;
      const auto ch2 = *it2;
      if (ch0 == ch1 && ch1 == ch2) {
        ch0 = 0;
        continue;
      }
      if (++pair_counts[pair2index(ch1, ch2)] == 2) {
        break;
      }
      ch0 = ch1;
    }
  }
  return std::ranges::any_of(pair_counts, [](auto n) { return n >= 2; });
}

bool is_nice_part2(const std::string& s) {
  return contains_sandwich_letter(s) && contains_letter_pair_twice(s);
}

int main() {
  const auto lines = std::views::istream<std::string>(std::cin) |
                     // TODO(llvm17)
                     // std::ranges::to<std::vector<int>>();
                     my_ranges::to<std::vector<std::string>>();
  const auto part1{std::ranges::count_if(lines, is_nice_part1)};
  const auto part2{std::ranges::count_if(lines, is_nice_part2)};
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
