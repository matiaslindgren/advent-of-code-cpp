import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Token : char {
  north = 'N',
  east = 'E',
  south = 'S',
  west = 'W',
  lpar = '(',
  rpar = ')',
  pipe = '|',
  begin = '^',
  end = '$',
};

std::istream& operator>>(std::istream& is, Token& token) {
  if (std::underlying_type_t<Token> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Token::north):
      case std::to_underlying(Token::east):
      case std::to_underlying(Token::south):
      case std::to_underlying(Token::west):
      case std::to_underlying(Token::lpar):
      case std::to_underlying(Token::rpar):
      case std::to_underlying(Token::pipe):
      case std::to_underlying(Token::begin):
      case std::to_underlying(Token::end): {
        token = {ch};
      } break;
      default: {
        is.setstate(std::ios_base::failbit);
      } break;
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Token");
}

using Vec2 = aoc::Vec2<int>;
using Steps = std::vector<std::pair<Vec2, Vec2>>;

auto find_shortest_paths(const Steps& steps) {
  std::unordered_map<Vec2, std::unordered_set<Vec2>> edges;
  for (auto&& [src, dst] : steps) {
    if (src != dst) {
      edges[src].insert(dst);
      edges[dst].insert(src);
    }
  }

  std::unordered_map<Vec2, int> dist;
  for (std::deque q{Vec2{}}; not q.empty(); q.pop_front()) {
    const auto src{q.front()};
    for (Vec2 dst : src.adjacent()) {
      if (not dist.contains(dst)) {
        if (edges[src].contains(dst)) {
          dist[dst] += 1 + dist[src];
          q.emplace_back(dst);
        }
      }
    }
  }
  return dist;
}

Steps walk(const auto& tokens) {
  Steps steps;
  const std::tuple init_state{
      tokens.begin(),
      Vec2{},
      Vec2{},
      Steps{},
  };
  for (std::deque q{init_state}; not q.empty(); q.pop_front()) {
    auto [token, p0, p1, branches]{q.front()};
    if (token == tokens.end()) {
      throw std::runtime_error("invalid regex, it must end with '$'");
    }
    steps.emplace_back(p0, p1);
    switch (*token) {
      case Token::north: {
        q.emplace_back(++token, p1, p1 + Vec2(0, -1), branches);
      } break;
      case Token::east: {
        q.emplace_back(++token, p1, p1 + Vec2(1, 0), branches);
      } break;
      case Token::south: {
        q.emplace_back(++token, p1, p1 + Vec2(0, 1), branches);
      } break;
      case Token::west: {
        q.emplace_back(++token, p1, p1 + Vec2(-1, 0), branches);
      } break;
      case Token::lpar: {
        branches.emplace_back(p0, p1);
        q.emplace_back(++token, p0, p1, branches);
      } break;
      case Token::rpar: {
        if (branches.empty()) {
          throw std::runtime_error("invalid regex, missing open group '('");
        }
        branches.pop_back();
        q.emplace_back(++token, p0, p1, branches);
      } break;
      case Token::pipe: {
        if (branches.empty()) {
          throw std::runtime_error("invalid regex, cannot have pipe '|' without open group '('");
        }
        const auto [root_prev, root]{branches.back()};
        q.emplace_back(++token, root_prev, root, branches);
      } break;
      case Token::begin: {
        q.emplace_back(++token, p0, p1, branches);
      } break;
      case Token::end: {
      } break;
    }
  }
  return steps;
}

auto search_paths(const auto& tokens) {
  const auto steps{walk(tokens)};
  const auto dist{find_shortest_paths(steps)};
  const auto lengths{views::values(dist)};
  return std::pair{
      my_std::ranges::fold_left(lengths, 0, ranges::max),
      ranges::count_if(lengths, [](auto&& len) { return len >= 1'000; }),
  };
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto tokens{views::istream<Token>(input) | ranges::to<std::vector>()};

  const auto [part1, part2]{search_paths(tokens)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
