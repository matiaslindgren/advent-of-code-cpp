#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using aoc::skip;
using std::operator""s;

auto reversed(ranges::range auto&& r) {
  auto res{r};
  ranges::reverse(res);
  return res;
}

struct Image {
  long width{};
  std::string str;

  [[nodiscard]]
  auto height() const {
    return ranges::ssize(str) / width;
  }

  auto row(this auto&& self, std::size_t y) {
    return self.str | views::drop(y * self.width) | views::take(self.width);
  }

  [[nodiscard]]
  auto yx_range() const {
    return my_std::views::cartesian_product(views::iota(0L, height()), views::iota(0L, width));
  }

  [[nodiscard]]
  auto flip_y(auto y) const {
    return height() - y - 1;
  }

  [[nodiscard]]
  auto flip_x(auto x) const {
    return width - x - 1;
  }
};

struct Tile {
  int id{};
  Image img;
  std::array<std::string, 4> borders{};

  [[nodiscard]]
  Tile rotate() const {
    Tile out{*this};
    for (auto [y, x] : img.yx_range()) {
      out.img.row(y)[img.flip_x(x)] = img.row(x)[y];
    }
    for (auto&& [side, border] : my_std::views::enumerate(out.borders)) {
      border = borders.at((side + 3) % borders.size());
    }
    return out;
  }

  [[nodiscard]]
  Tile flip() const {
    Tile out{*this};
    for (auto [y, x] : img.yx_range()) {
      out.img.row(img.flip_y(y))[x] = img.row(y)[x];
    }
    for (auto&& [side, border] : my_std::views::enumerate(out.borders)) {
      border = reversed(borders.at((2 - side + borders.size()) % borders.size()));
    }
    return out;
  }

  [[nodiscard]]
  std::optional<Tile> find_matching_orientation(Tile other, int side) const {
    for (int flips{}; flips < 2; ++flips) {
      for (int rotations{}; rotations < 4; ++rotations) {
        auto lhs{borders.at(side)};
        auto rhs{other.borders.at((side + 2) % borders.size())};
        if (ranges::equal(lhs, rhs | views::reverse)) {
          return other;
        }
        other = other.rotate();
      }
      other = other.flip();
    }
    return {};
  }
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;

  Grid(Tile t1, Vec2 p, const auto& tilemap) {
    std::unordered_set<int> frozen;
    *this = build(t1, p, tilemap, frozen);
  }

 private:
  Grid build(Tile t1, Vec2 p, const auto& tilemap, auto& frozen) {
    constexpr std::array<Vec2, 4> deltas{{
        Vec2(0, -1),
        Vec2(1, 0),
        Vec2(0, 1),
        Vec2(-1, 0),
    }};
    Grid out{*this};
    frozen.insert(t1.id);
    out.tiles[p] = t1;
    for (int side{}; side < deltas.size(); ++side) {
      for (Tile t : tilemap | views::values) {
        if (not frozen.contains(t.id)) {
          if (auto match{t1.find_matching_orientation(t, side)}) {
            Tile t2{match.value()};
            Vec2 p2{p + deltas.at(side)};
            out = out.build(t2, p2, tilemap, frozen);
          }
        }
      }
    }
    return out;
  }

 public:
  [[nodiscard]]
  auto corners() const {
    auto ps{views::keys(tiles)};
    return std::array{
        ranges::min(ps, {}, [](const Vec2& p) { return std::tuple{p.x(), p.y()}; }),
        ranges::max(ps, {}, [](const Vec2& p) { return std::tuple{p.x(), -p.y()}; }),
        ranges::min(ps, {}, [](const Vec2& p) { return std::tuple{p.x(), -p.y()}; }),
        ranges::max(ps, {}, [](const Vec2& p) { return std::tuple{p.x(), p.y()}; }),
    };
  }

  [[nodiscard]]
  Image as_image() const {
    if (tiles.empty()) {
      throw std::runtime_error("cannot build image of empty grid");
    }
    const auto c{corners()};
    const Vec2 top_left{c[0]};
    const Vec2 bottom_right{c[3]};
    const auto h{tiles.begin()->second.img.height()};
    Image img;
    for (auto p{top_left}; p.y() <= bottom_right.y(); ++p.y()) {
      for (auto y_img{0UZ}; y_img < h; ++y_img) {
        std::string row;
        for (p.x() = top_left.x(); p.x() <= bottom_right.x(); ++p.x()) {
          if (tiles.contains(p)) {
            row.append_range(tiles.at(p).img.row(y_img));
          }
        }
        if (row.empty()) {
          continue;
        }
        if (img.width == 0) {
          img.width = ranges::ssize(row);
        } else if (img.width != row.size()) {
          throw std::runtime_error("all rows in image must be of same width");
        }
        img.str += row;
      }
    }
    return img;
  }
};

constexpr std::array monster_pattern{
    "..................#."s,
    "#....##....##....###"s,
    ".#..#..#..#..#..#..."s,
};

auto count_monsters(Image img) {
  int n{};
  const auto is_match{[](auto row, auto begin, auto pattern) {
    return ranges::all_of(views::zip(row | views::drop(begin), pattern), [](auto&& p) {
      auto [lhs, rhs]{p};
      return rhs != '#' or lhs == rhs;
    });
  }};
  const auto monster_width{monster_pattern[0].size()};
  for (auto r{2UZ}; r < img.height(); ++r) {
    auto row1{img.row(r - 2)};
    auto row2{img.row(r - 1)};
    auto row3{img.row(r - 0)};
    if (row1.size() < monster_width) {
      throw std::runtime_error("row too short to contain a monster");
    }
    for (auto begin{0UZ}; begin <= row1.size() - monster_width; ++begin) {
      if (is_match(row1, begin, monster_pattern[0]) and is_match(row2, begin, monster_pattern[1])
          and is_match(row3, begin, monster_pattern[2])) {
        ++n;
        begin += monster_width;
      }
    }
  }
  return n;
}

auto find_monster_grid(const auto& tilemap) {
  for (Tile t : tilemap | views::values) {
    for (int flips{}; flips < 2; ++flips) {
      for (int rotations{}; rotations < 4; ++rotations) {
        Grid g(t, Vec2(), tilemap);
        if (g.tiles.size() == tilemap.size()) {
          if (auto img{g.as_image()}; count_monsters(img) > 0) {
            return std::pair{g, img};
          }
        }
        t = t.rotate();
      }
      t = t.flip();
    }
  }
  throw std::runtime_error("failed finding monster grid");
}

constexpr auto product{std::bind_back(ranges::fold_left, 1LL, std::multiplies{})};

auto search(const auto& tilemap) {
  auto [grid, img]{find_monster_grid(tilemap)};
  auto corner_id{product(views::transform(grid.corners(), [&grid](const Vec2& p) {
    return grid.tiles.at(p).id;
  }))};
  auto monsters{count_monsters(img)};
  auto monster_hashes{ranges::count(monster_pattern | views::join, '#')};
  auto hashes{ranges::count(img.str, '#')};
  auto roughness{hashes - monsters * monster_hashes};
  return std::pair{corner_id, roughness};
}

std::istream& operator>>(std::istream& is, Tile& tile) {
  if (int id{}; is >> std::ws >> skip("Tile"s) >> id >> skip(":"s) >> std::ws) {
    std::vector<std::string> lines;
    for (std::string line; std::getline(is, line) and not line.empty();) {
      if (line.size() < 3) {
        throw std::runtime_error("every line must have at least 3 characters");
      }
      if (not lines.empty() and lines.back().size() != line.size()) {
        throw std::runtime_error("every line must be of same length");
      }
      lines.push_back(line);
    }
    if (lines.size() < 3) {
      throw std::runtime_error("every tile must have at least 3 rows");
    }

    std::string north;
    std::string east;
    std::string south;
    std::string west;

    north = lines.front();
    for (const std::string& line : lines) {
      west.push_back(line.front());
      east.push_back(line.back());
    }
    west = reversed(west);
    south = reversed(lines.back());

    const auto without_borders{[](ranges::sized_range auto&& r) {
      return r | views::take(r.size() - 1) | views::drop(1);
    }};
    Image img{.width = ranges::ssize(south) - 2};
    for (const std::string& line : without_borders(lines)) {
      img.str.append_range(without_borders(line));
    }

    tile = {
        .id = id,
        .img = img,
        .borders = {north, east, south, west},
    };
  }
  return is;
}

int main() {
  const auto tiles{aoc::parse_items<Tile>("/dev/stdin")};
  const auto tilemap{
      tiles | views::transform([](Tile t) { return std::pair{t.id, t}; })
      | ranges::to<std::unordered_map<int, Tile>>()
  };
  const auto [part1, part2]{search(tilemap)};
  std::println("{} {}", part1, part2);
  return 0;
}
