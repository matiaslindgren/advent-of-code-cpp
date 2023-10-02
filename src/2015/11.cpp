import std;

constexpr static auto alphabet_size{'z' - 'a' + 1};

constexpr int char2digit(char ch) {
  return ch - 'a';
}
constexpr char digit2char(int x) {
  return x + 'a';
}

constexpr static auto forbidden_chars =
    std::views::transform("iol", char2digit);

bool valid(const auto& password) {
  const auto begin{password.begin()};
  const auto end{password.end()};

  if (std::ranges::find_first_of(password, forbidden_chars) != end) {
    return false;
  }

  {
    std::bitset<alphabet_size> pairs;
    for (auto it{begin}; it != end; std::ranges::advance(it, 2, end)) {
      it = std::ranges::adjacent_find(it, end);
      if (it != end) {
        pairs[*it] = true;
      }
    }
    if (pairs.count() < 2) {
      return false;
    }
  }

  // TODO(llvm18)
  // std::views::adjacent<3>(password) ...
  return std::ranges::any_of(std::views::zip(password,
                                             std::views::drop(password, 1),
                                             std::views::drop(password, 2)),
                             [](const auto& t) {
                               const auto& [x0, x1, x2] = t;
                               return x0 + 1 == x1 && x1 + 1 == x2;
                             });
}

template <auto base>
auto increment(const auto& input) {
  std::vector<int> result(input.size());
  int carry{1};
  for (const auto& t : std::views::zip(std::views::reverse(result),
                                       std::views::reverse(input))) {
    auto& [dst, src] = t;
    dst = src + carry;
    carry = dst == base;
    dst %= base;
  }
  return result;
}

auto to_string(const auto& password) {
  return std::views::transform(password, digit2char) |
         std::ranges::to<std::string>();
}

int main() {
  std::ios_base::sync_with_stdio(false);

  auto password = std::views::istream<char>(std::cin) |
                  std::views::transform(char2digit) |
                  std::ranges::to<std::vector<int>>();

  std::string results[2] = {};
  for (auto& result : results) {
    do {
      password = increment<alphabet_size>(password);
    } while (!valid(password));
    result = to_string(password);
  }

  std::print("{} {}\n", results[0], results[1]);

  return 0;
}
