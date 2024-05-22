#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

signed char char2digit(char ch) {
  return static_cast<signed char>(ch - 'a');
}
char digit2char(signed char x) {
  return static_cast<char>(x + 'a');
}

constexpr static auto alphabet_size{'z' - 'a' + 1};
constexpr static auto forbidden_chars{views::transform("iol", char2digit)};

bool valid(const auto& password) {
  const auto begin{password.begin()};
  const auto end{password.end()};

  if (ranges::find_first_of(password, forbidden_chars) != end) {
    return false;
  }

  {
    std::bitset<alphabet_size> pairs;
    for (auto it{begin}; it != end; ranges::advance(it, 2, end)) {
      it = ranges::adjacent_find(it, end);
      if (it != end) {
        pairs[*it] = true;
      }
    }
    if (pairs.count() < 2) {
      return false;
    }
  }

  // TODO(llvm19)
  // std::views::adjacent<3>(password) ...
  return ranges::any_of(
      views::zip(password, views::drop(password, 1), views::drop(password, 2)),
      my_std::apply_fn([](auto x0, auto x1, auto x2) { return x0 + 1 == x1 and x1 + 1 == x2; })
  );
}

std::string search(auto& password) {
  do {
    std::vector<signed char> result(password.size());
    signed char carry{1};
    for (auto&& [dst, src] : views::reverse(views::zip(result, password))) {
      dst = src + carry;
      carry = dst == alphabet_size;
      dst %= alphabet_size;
    }
    password = result;
  } while (not valid(password));
  return views::transform(password, digit2char) | ranges::to<std::string>();
}

int main() {
  auto password{
      aoc::parse_items<char>("/dev/stdin") | views::transform(char2digit)
      | ranges::to<std::vector>()
  };

  const auto part1{search(password)};
  const auto part2{search(password)};

  std::println("{} {}", part1, part2);

  return 0;
}
