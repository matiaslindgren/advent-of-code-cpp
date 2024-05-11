#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

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

int main() {
  const auto items{aoc::slurp<Item>("/dev/stdin")};

  ranges::copy(items, std::ostream_iterator<Item>(std::cout, "\n"));

  const auto part1{find_part1(items)};
  const auto part2{find_part2(items)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
