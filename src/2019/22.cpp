#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Shuffle {
  enum {
    rotate_left,
    rotate_right,
    increment,
    into_new,
  } type;
  int n{};
};

std::istream& operator>>(std::istream& is, Shuffle& s) {
  if (std::string tmp; is >> std::ws >> tmp) {
    if (tmp == "cut") {
      if (int n; is >> n) {
        s = {n < 0 ? Shuffle::rotate_right : Shuffle::rotate_left, std::abs(n)};
      }
    } else if (tmp == "deal") {
      if (is >> tmp >> std::ws) {
        if (int n; tmp == "with" and is >> skip("increment"s) >> n) {
          s = {Shuffle::increment, n};
        } else if (tmp == "into" and is >> skip("new stack"s)) {
          s = {Shuffle::into_new};
        } else {
          throw std::runtime_error("deal must be followed by 'with increment' or 'into new stack'");
        }
      }
    } else {
      throw std::runtime_error("first word must be cut or deal");
    }
  }
  return is;
}

auto find_part1(const auto& shuffles) {
  std::array<int, 10007> cards;
  // todo ranges::iota
  for (auto&& [i, c] : my_std::views::enumerate(cards)) {
    c = i;
  }
  constexpr auto n{cards.size()};
  for (auto s : shuffles) {
    switch (s.type) {
      case Shuffle::rotate_left: {
        ranges::rotate(cards, cards.begin() + s.n);
      } break;
      case Shuffle::rotate_right: {
        ranges::rotate(cards, cards.end() - s.n);
      } break;
      case Shuffle::increment: {
        const auto prev_cards{cards};
        for (auto&& [i, c] : my_std::views::enumerate(prev_cards)) {
          cards[(i * s.n) % n] = c;
        }
      } break;
      case Shuffle::into_new: {
        ranges::reverse(cards);
      } break;
    }
  }
  return ranges::find(cards, 2019) - cards.begin();
}

using Int = __int128;

Int powmod(Int base, Int exp, Int mod) {
  // https://en.wikipedia.org/wiki/Modular_exponentiation
  // accessed 2024-03-28
  if (mod == 1) {
    return Int{};
  }
  Int res{1};
  for (base %= mod; exp > 0; exp /= 2) {
    if (exp % 2 == 1) {
      res = (res * base) % mod;
    }
    base = (base * base) % mod;
  }
  return res;
}

Int modinv_prime(Int x, Int p) {
  return powmod(x, p - 2, p);
}

constexpr Int n_iter{101741582076661};
constexpr Int n_cards{119315717514047};

// part 2 math from
// https://www.reddit.com/r/adventofcode/comments/ee0rqi/comment/fbnifwk
// accessed 2024-03-28

auto reverse_deal(auto i) {
  return n_cards - i - 1;
}

auto reverse_cut(auto i, auto n) {
  return (i + n + n_cards) % n_cards;
}

auto reverse_inc(auto i, auto n) {
  return (modinv_prime(n, n_cards) * i) % n_cards;
}

auto apply_reverse(auto x, const auto& shuffles) {
  for (auto s : views::reverse(shuffles)) {
    switch (s.type) {
      case Shuffle::rotate_left: {
        x = reverse_cut(x, s.n);
      } break;
      case Shuffle::rotate_right: {
        x = reverse_cut(x, -s.n);
      } break;
      case Shuffle::increment: {
        x = reverse_inc(x, s.n);
      } break;
      case Shuffle::into_new: {
        x = reverse_deal(x);
      } break;
    }
  }
  return x;
}

auto find_part2(const auto& shuffles) {
  constexpr auto n{n_cards};

  Int x{2020};
  Int y{apply_reverse(x, shuffles)};
  Int z{apply_reverse(y, shuffles)};

  auto a{((y - z) * modinv_prime(x - y, n)) % n};
  auto b{(n + (y - a * x) % n) % n};
  auto a_pow{powmod(a, n_iter, n)};

  auto res1{a_pow * x};
  auto res2{(a_pow - 1) * ((modinv_prime(a - 1, n) * b) % n)};
  return (res1 % n + res2 % n) % n;
}

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  auto shuffles{views::istream<Shuffle>(is) | ranges::to<std::vector>()};
  if (is.eof()) {
    return shuffles;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto shuffles{parse_input("/dev/stdin")};

  const auto part1{find_part1(shuffles)};
  const auto part2{find_part2(shuffles)};

  std::println("{} {}", part1, part2);

  return 0;
}
