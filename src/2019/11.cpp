import std;
import aoc;
import intcode;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using intcode::IntCode;
using Vec2 = aoc::Vec2<int>;

enum class Tile : int {
  white = 0,
  black = 1,
};

using Tiles = std::unordered_map<Vec2, Tile>;

Tiles paint(Tiles tiles, const auto& program) {
  for (auto [ic, pos, dir]{std::tuple{IntCode(program), Vec2(0, 0), Vec2(0, -1)}}; not ic.is_done();
       pos += dir) {
    ic.push_input(tiles.contains(pos) and tiles.at(pos) == Tile::white);

    ic.run_until_output();
    if (auto paint{ic.pop_output()}) {
      switch (paint.value()) {
        case 0: {
          tiles[pos] = Tile::black;
        } break;
        case 1: {
          tiles[pos] = Tile::white;
        } break;
        default: {
          throw std::runtime_error("invalid intcode output, can't paint");
        } break;
      }
    }

    ic.run_until_output();
    if (auto turn{ic.pop_output()}) {
      switch (turn.value()) {
        case 0: {
          dir.rotate_left();
        } break;
        case 1: {
          dir.rotate_right();
        } break;
        default: {
          throw std::runtime_error("invalid intcode output, can't turn");
        } break;
      }
    }
  }
  return tiles;
}

std::string parse_identifier(const Tiles& tiles) {
  std::string id;
  const std::size_t w{4}, h{6};
  for (auto i_letter{0uz}; i_letter < 8; ++i_letter) {
    std::string letter;
    for (auto y{0uz}; y < h; ++y) {
      for (auto x{0uz}; x < w; ++x) {
        const Vec2 p(x + 1 + i_letter * (w + 1), y);
        letter.push_back(tiles.contains(p) and tiles.at(p) == Tile::white ? '#' : '.');
      }
    }
    id.push_back(aoc::ocr(letter));
  }
  return id;
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto tiles1{paint(Tiles{}, program)};
  const auto part1{tiles1.size()};

  const auto tiles2{paint(Tiles{{Vec2(), Tile::white}}, program)};
  const auto part2{parse_identifier(tiles2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
