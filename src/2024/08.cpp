#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

auto search(const auto& grid, bool part2) {
  std::unordered_set<Vec2> antinodes;
  for (const auto& signals : grid.antennas | views::values) {
    for (auto p1{signals.begin()}; p1 != signals.end(); ++p1) {
      if (part2) {
        antinodes.insert(*p1);
      }
      for (auto p2{p1 + 1}; p2 != signals.end(); ++p2) {
        Vec2 diff{*p1 - *p2};
        for (Vec2 anti1{*p1 + diff}; grid.tiles.contains(anti1); anti1 += diff) {
          antinodes.insert(anti1);
          if (not part2) {
            break;
          }
        }
        for (Vec2 anti2{*p2 - diff}; grid.tiles.contains(anti2); anti2 -= diff) {
          antinodes.insert(anti2);
          if (not part2) {
            break;
          }
        }
      }
    }
  }
  return antinodes.size();
}

struct Grid {
  std::unordered_map<Vec2, char> tiles;
  std::unordered_map<char, std::vector<Vec2>> antennas;
  std::size_t width;
  std::size_t height;
};

auto parse_grid(std::string_view path) {
  Grid g{};
  Vec2 p;
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (g.width == 0U) {
      g.width = line.size();
    } else if (g.width != line.size()) {
      throw std::runtime_error("every row must be of equal length");
    }
    for (p.x() = 0; char ch : line) {
      if (ch == '.' or ('0' <= ch and ch <= '9') or ('a' <= ch and ch <= 'z')
          or ('A' <= ch and ch <= 'Z')) {
        g.tiles[p] = {ch};
        if (ch != '.') {
          g.antennas[ch].push_back(p);
        }
      } else {
        throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
  }
  g.height = p.y();
  return g;
}

int main() {
  const Grid grid{parse_grid("/dev/stdin")};

  const auto part1{search(grid, false)};
  const auto part2{search(grid, true)};

  std::println("{} {}", part1, part2);

  return 0;
}
