import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Move {
  enum {
    spin,
    exchange,
    partner,
  } type;
  int a{}, b{};
};

std::istream& operator>>(std::istream& is, Move& move) {
  if (char type; is >> type) {
    if (int s; type == 's' and is >> s) {
      move = {Move::spin, s};
    } else if (int a, b; type == 'x' and is >> a >> skip("/"s) >> b) {
      move = {Move::exchange, a, b};
    } else if (char a, b; type == 'p' and is >> a >> skip("/"s) >> b) {
      move = {Move::partner, a, b};
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Move");
}

std::string dance(const auto& moves, const int rounds) {
  std::vector<std::string> seen;
  for (auto s{"abcdefghijklmnop"s}; ranges::find(seen, s) == seen.end();) {
    seen.push_back(s);
    for (const auto& move : moves) {
      switch (move.type) {
        case Move::spin: {
          ranges::rotate(s, s.end() - (move.a % s.size()));
        } break;
        case Move::exchange: {
          std::swap(s.at(move.a), s.at(move.b));
        } break;
        case Move::partner: {
          std::swap(s.at(s.find(move.a)), s.at(s.find(move.b)));
        } break;
      }
    }
  }
  return seen.at(rounds % seen.size());
}

auto parse_moves(std::string path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  return views::istream<Move>(is) | ranges::to<std::vector>();
}

int main() {
  const auto moves{parse_moves("/dev/stdin")};

  const auto part1{dance(moves, 1)};
  const auto part2{dance(moves, 1'000'000'000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
