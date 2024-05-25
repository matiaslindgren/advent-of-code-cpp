#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

enum class Tile : char {
  NS = '|',
  EW = '-',
  NE = 'L',
  NW = 'J',
  SW = '7',
  SE = 'F',
  ground = '.',
  start = 'S',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  std::size_t width;

  [[nodiscard]]
  bool is_connected(const Vec2& src, const Vec2& dst) const {
    if (not tiles.contains(src) or not tiles.contains(dst)) {
      return false;
    }
    if (dst < src) {
      return is_connected(dst, src);
    }
    const Tile t1{tiles.at(src)};
    const Tile t2{tiles.at(dst)};
    const Vec2 diff{(dst - src)};
    if (diff == Vec2(1, 0)) {
      return (t1 == Tile::EW or t1 == Tile::NE or t1 == Tile::SE or t1 == Tile::start)
             and (t2 == Tile::EW or t2 == Tile::NW or t2 == Tile::SW or t2 == Tile::start);
    }
    if (diff == Vec2(0, 1)) {
      return (t1 == Tile::NS or t1 == Tile::SW or t1 == Tile::SE or t1 == Tile::start)
             and (t2 == Tile::NS or t2 == Tile::NE or t2 == Tile::NW or t2 == Tile::start);
    }
    return false;
  }

  [[nodiscard]]
  auto count_adjacent(Vec2 p) const {
    return ranges::count_if(p.adjacent(), [p, this](Vec2 adj) { return is_connected(p, adj); });
  }

  [[nodiscard]]
  auto walk_loop(Vec2 start) const {
    std::unordered_set<Vec2> path{{start}};
    for (Vec2 src{start};;) {
      Vec2 prev{src};
      for (Vec2 dst : src.adjacent()) {
        if (not path.contains(dst) and is_connected(src, dst)) {
          src = dst;
          break;
        }
      }
      if (prev == src) {
        return path;
      }
      path.insert(src);
    }
  }

  void infer_tile_at(Vec2 p) {
    for (Tile t : {Tile::NS, Tile::EW, Tile::NE, Tile::NW, Tile::SW, Tile::SE}) {
      Tile prev{std::exchange(tiles.at(p), t)};
      if (count_adjacent(p) == 2) {
        return;
      }
      tiles.at(p) = prev;
    }
  }
};

auto count_inner(Grid grid, const auto& path, Vec2 start) {
  grid.infer_tile_at(start);
  for (auto&& [pos, tile] : grid.tiles) {
    if (not path.contains(pos)) {
      tile = Tile::ground;
    }
  }
  auto row_major_points{views::keys(grid.tiles) | ranges::to<std::vector>()};
  ranges::sort(row_major_points, {}, [](const Vec2& p) { return std::tuple{p.y(), p.x()}; });
  return ranges::count_if(
      row_major_points,
      [&, prev_angle = Tile::ground, is_in = false](const Vec2& p) mutable {
        if (p.x() == 0) {
          prev_angle = Tile::ground;
          is_in = false;
        }
        Tile t{grid.tiles.at(p)};
        if (t == Tile::ground) {
          return is_in;
        }
        const bool should_flip{
            t == Tile::NS or (t == Tile::NW and prev_angle == Tile::SE)
            or (t == Tile::SW and prev_angle == Tile::NE)
        };
        if (should_flip) {
          is_in = not is_in;
        } else if (t == Tile::NE or t == Tile::SE) {
          prev_angle = t;
        }
        return false;
      }
  );
}

auto search(Grid grid, Vec2 start) {
  auto path{grid.walk_loop(start)};
  return std::pair{
      path.size() / 2,
      count_inner(grid, path, start),
  };
}

auto parse_grid(std::string_view path) {
  Grid g{};
  std::optional<Vec2> start;
  {
    Vec2 p;
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
      if (g.width == 0U) {
        g.width = line.size();
      } else if (g.width != line.size()) {
        throw std::runtime_error("every row must be of equal length");
      }
      p.x() = 0;
      for (char ch : line) {
        switch (ch) {
          case std::to_underlying(Tile::NS):
          case std::to_underlying(Tile::EW):
          case std::to_underlying(Tile::NE):
          case std::to_underlying(Tile::NW):
          case std::to_underlying(Tile::SW):
          case std::to_underlying(Tile::SE):
          case std::to_underlying(Tile::ground):
          case std::to_underlying(Tile::start): {
            g.tiles[p] = {ch};
          } break;
          default: {
            throw std::runtime_error(std::format("unknown tile '{}'", ch));
          }
        }
        if (not start and g.tiles.at(p) == Tile::start) {
          start = p;
        }
        p.x() += 1;
      }
    }
  }
  if (not start) {
    throw std::runtime_error("input does not contain start S");
  }
  return std::pair{g, start.value()};
}

int main() {
  const auto [grid, start]{parse_grid("/dev/stdin")};
  const auto [part1, part2]{search(grid, start)};
  std::println("{} {}", part1, part2);
  return 0;
}
