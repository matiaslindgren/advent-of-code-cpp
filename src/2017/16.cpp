#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Move {
  enum : unsigned char {
    spin,
    exchange,
    partner,
  } type{};
  int a{};
  int b{};
};

std::istream& operator>>(std::istream& is, Move& move) {
  if (char type{}; is >> type) {
    if (int s{}; type == 's' and is >> s) {
      move = {Move::spin, s};
    } else if (int a{}, b{}; type == 'x' and is >> a >> skip("/"s) >> b) {
      move = {Move::exchange, a, b};
    } else if (char a{}, b{}; type == 'p' and is >> a >> skip("/"s) >> b) {
      move = {Move::partner, a, b};
    } else {
      throw std::runtime_error("failed parsing move type");
    }
  }
  return is;
}

std::string dance(const auto& moves, const int rounds) {
  std::vector<std::string> seen;
  for (auto s{"abcdefghijklmnop"s}; ranges::find(seen, s) == seen.end();) {
    seen.push_back(s);
    for (const Move& move : moves) {
      switch (move.type) {
        case Move::spin: {
          ranges::rotate(s, s.end() - (move.a % ranges::ssize(s)));
        } break;
        case Move::exchange: {
          std::swap(s.at(move.a), s.at(move.b));
        } break;
        case Move::partner: {
          auto a{static_cast<char>(move.a)};
          auto b{static_cast<char>(move.b)};
          std::swap(s.at(s.find(a)), s.at(s.find(b)));
        } break;
      }
    }
  }
  return seen.at(rounds % seen.size());
}

int main() {
  const auto moves{aoc::parse_items<Move>("/dev/stdin", ',')};

  const auto part1{dance(moves, 1)};
  const auto part2{dance(moves, 1'000'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
