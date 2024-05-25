#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

struct Item {
  int id;
  std::string str;
};

auto find_part1(const auto& items) {
  return 0;
}

auto find_part2(const auto& items) {
  return 0;
}

template <>
struct std::formatter<Item, char> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Item& item, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "Item {{ {} {} }}", item.id, item.str);
  }
};

std::istream& operator>>(std::istream& is, Item& item) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (int id; ls >> id) {
      if (std::string str; ls >> str) {
        item = {id, str};
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

std::ostream& operator<<(std::ostream& os, const Item& item) {
  return os << std::format("{}", item);
}

enum class Tile : char {
  rock = '#',
  none = '.',
};

struct Grid {
  std::unordered_map<Vec2, Tile> tiles;
  std::size_t width;
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
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case std::to_underlying(Tile::rock):
        case std::to_underlying(Tile::none): {
          g.tiles[p] = {ch};
        } break;
        default: {
          throw std::runtime_error(std::format("unknown tile '{}'", ch));
        }
      }
      p.x() += 1;
    }
  }
  return g;
}

int main() {
  const auto items{aoc::parse_items<Item>("/dev/stdin")};

  ranges::copy(items, std::ostream_iterator<Item>(std::cout, "\n"));

  const auto part1{find_part1(items)};
  const auto part2{find_part2(items)};

  std::println("{} {}", part1, part2);

  return 0;
}
