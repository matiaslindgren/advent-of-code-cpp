#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum struct Direction : unsigned char {
  north,
  south,
  west,
  east,
};

auto next(Direction d, int turn = 1) {
  return Direction{static_cast<unsigned char>((std::to_underlying(d) + turn) % 4)};
}

struct Grid {
  std::unordered_set<Vec2> elves;

  [[nodiscard]]
  bool has_adjacent_elf(Vec2 pos, Vec2 dir) const {
    for (Vec2 d(-1, -1); d.y() <= 1; d.y() += 1) {
      for (d.x() = -1; d.x() <= 1; d.x() += 1) {
        if ((dir.x() == 0 or d.x() == dir.x()) and (dir.y() == 0 or d.y() == dir.y())) {
          if (Vec2 adj{pos + d}; adj != pos and elves.contains(adj)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  [[nodiscard]]
  bool has_adjacent_elves(Vec2 pos) const {
    return ranges::any_of(
        std::array{Vec2(0, 1), Vec2(0, -1), Vec2(1, 0), Vec2(-1, 0)},
        [pos, this](Vec2 dir) { return has_adjacent_elf(pos, dir); }
    );
  }

  [[nodiscard]]
  std::optional<Vec2> find_move(Vec2 pos, Direction start_dir) const {
    for (int turn{}; turn < 4; ++turn) {
      Vec2 dir;
      switch (next(start_dir, turn)) {
        case Direction::north: {
          dir = Vec2(0, -1);
        } break;
        case Direction::south: {
          dir = Vec2(0, 1);
        } break;
        case Direction::west: {
          dir = Vec2(-1, 0);
        } break;
        case Direction::east: {
          dir = Vec2(1, 0);
        } break;
      }
      if (not has_adjacent_elf(pos, dir)) {
        return pos + dir;
      }
    }
    return std::nullopt;
  }
};

auto find_part1(const Grid& g) {
  const auto [p_min, p_max]{ranges::fold_left(
      g.elves,
      std::pair{Vec2(), Vec2()},
      [](auto minmax, const Vec2& p) {
        return std::pair{minmax.first.min(p), minmax.second.max(p)};
      }
  )};
  int n{};
  for (Vec2 p{p_min}; p.y() <= p_max.y(); p.y() += 1) {
    for (p.x() = p_min.x(); p.x() <= p_max.x(); p.x() += 1) {
      n += int{not g.elves.contains(p)};
    }
  }
  return n;
}

auto search(Grid grid) {
  std::optional<int> part1{};
  std::optional<int> part2{};

  auto dir{Direction::north};
  for (int round{}; round < 10'000 and (not part1 or not part2); ++round) {
    std::unordered_map<Vec2, Vec2> moves;
    std::unordered_map<Vec2, int> target_counts;

    for (Vec2 pos : grid.elves) {
      if (grid.has_adjacent_elves(pos)) {
        if (auto dst{grid.find_move(pos, dir)}) {
          moves[pos] = *dst;
          target_counts[*dst] += 1;
        }
      }
    }

    for (auto [src, dst] : moves) {
      if (target_counts[dst] == 1) {
        grid.elves.erase(src);
        grid.elves.insert(dst);
      }
    }

    if (round == 10) {
      part1 = find_part1(grid);
    }
    if (ranges::count(views::values(target_counts), 1) == 0) {
      part2 = round + 1;
    }

    dir = next(dir);
  }

  return std::pair{part1.value(), part2.value()};
}

Grid parse_grid(std::string_view path) {
  Grid grid;
  for (Vec2 p; const std::string& line : aoc::slurp_lines(path)) {
    p.x() = 0;
    for (char ch : line) {
      if (ch == '#') {
        grid.elves.insert(p);
      } else if (ch != '.') {
        throw std::runtime_error(std::format("unknown grid tile '{}'", ch));
      }
      p.x() += 1;
    }
    p.y() += 1;
  }
  return grid;
}

int main() {
  const auto [part1, part2]{search(parse_grid("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
