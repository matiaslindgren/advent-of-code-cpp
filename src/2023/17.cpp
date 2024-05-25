#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Blocks {
  static constexpr auto max_loss{std::numeric_limits<int>::max()};
  std::unordered_map<Vec2, int> loss;
  long width{};
  long height{};

  [[nodiscard]]
  auto loss_at(const Vec2& p) const {
    return loss.contains(p) ? loss.at(p) : max_loss;
  }
};

constexpr std::array directions{
    Vec2(1, 0),
    Vec2(0, 1),
    Vec2(-1, 0),
    Vec2(0, -1),
};

struct State {
  Vec2 block;
  int direction{};
  int moves{};
};

constexpr auto search(const Blocks& blocks, const int min_moves, const int max_moves) {
  const auto n_moves{max_moves + 1};
  const auto n_directions{directions.size()};
  std::vector<int> min_loss(
      blocks.width * blocks.height * n_moves * n_directions,
      Blocks::max_loss
  );

  const auto loss{[&](const State& s) -> int& {
    return std::mdspan(
        min_loss.data(),
        blocks.width,
        blocks.height,
        n_moves,
        n_directions
    )[s.block.x(), s.block.y(), s.moves, s.direction];
  }};

  const auto push_min_loss_heap{[&](auto& q, const State& s) {
    q.push_back(s);
    ranges::push_heap(q, ranges::greater{}, loss);
  }};
  const auto pop_min_loss_heap{[&](auto& q) {
    ranges::pop_heap(q, ranges::greater{}, loss);
    State s{q.back()};
    q.pop_back();
    return s;
  }};

  const auto [begin, end]{ranges::minmax(views::keys(blocks.loss))};

  std::vector<State> q{
      {.block = begin, .direction = 0, .moves = 0},
      {.block = begin, .direction = 1, .moves = 0},
  };
  loss(q.at(0)) = 0;
  loss(q.at(1)) = 0;

  while (not q.empty()) {
    State src{pop_min_loss_heap(q)};
    if (src.block == end and min_moves <= src.moves and src.moves <= max_moves) {
      return loss(src);
    }
    for (int dir{}; dir < directions.size(); ++dir) {
      State dst{
          .block = src.block + directions.at(dir),
          .direction = dir,
      };
      if ((not blocks.loss.contains(dst.block))
          or ((dst.direction + 2) % directions.size() == src.direction)
          or (dst.direction != src.direction and src.moves < min_moves)
          or (dst.direction == src.direction and src.moves >= max_moves)) {
        continue;
      }
      dst.moves = (dst.direction == src.direction ? src.moves + 1 : 1);
      if (auto new_loss{aoc::saturating_add(loss(src), blocks.loss_at(dst.block))};
          new_loss < loss(dst)) {
        loss(dst) = new_loss;
        push_min_loss_heap(q, dst);
      }
    }
  }

  throw std::runtime_error("search failed, could not reach end");
}

auto parse_blocks(std::string_view path) {
  Blocks blocks{};
  Vec2 p;
  for (std::string_view line : aoc::slurp_lines(path)) {
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
          blocks.loss[p] = ch - '0';
        } break;
        default:
          throw std::runtime_error(std::format("all tiles must be digits, not '{}'", ch));
      }
      p.x() += 1;
    }
    if (blocks.width == 0) {
      blocks.width = p.x();
    } else if (blocks.width != p.x()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.y() += 1;
  }
  if (p == Vec2()) {
    throw std::runtime_error("empty input");
  }
  blocks.height = p.y();
  return blocks;
}

int main() {
  const Blocks blocks{parse_blocks("/dev/stdin")};

  const auto part1{search(blocks, 1, 3)};
  const auto part2{search(blocks, 4, 10)};

  std::println("{} {}", part1, part2);

  return 0;
}
