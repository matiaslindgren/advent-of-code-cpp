#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

// c++ version of
// https://www.reddit.com/r/adventofcode/comments/rizw2c/comment/hp15elp
// accessed 2024-05-04

struct Item {
  int value{};
  int depth{};
};

using Items = std::vector<Item>;
using Lines = std::vector<Items>;

bool explode(Items& items) {
  for (auto&& [i, item] : my_std::views::enumerate(items)) {
    if (const int depth{item.depth}; depth >= 5 and depth == items.at(i + 1).depth) {
      if (i > 0) {
        items.at(i - 1).value += item.value;
      }
      if (i < items.size() - 2) {
        items.at(i + 2).value += items.at(i + 1).value;
      }
      items.erase(items.begin() + i);
      items.erase(items.begin() + i);
      items.insert(items.begin() + i, Item{0, depth - 1});
      return true;
    }
  }
  return false;
}

bool split(Items& items) {
  for (auto&& [i, item] : my_std::views::enumerate(items)) {
    if (item.value > 9) {
      const int depth{item.depth};
      const int value1{item.value / 2};
      const int value2{item.value - value1};
      items.erase(items.begin() + i);
      items.insert(items.begin() + i, Item{value2, depth + 1});
      items.insert(items.begin() + i, Item{value1, depth + 1});
      return true;
    }
  }
  return false;
}

Items add(const Items& lhs, const Items& rhs) {
  Items result{lhs};
  result.append_range(rhs);
  ranges::for_each(result, [](Item& i) { i.depth += 1; });

  for (;;) {
    if (explode(result)) {
      continue;
    }
    if (split(result)) {
      continue;
    }
    break;
  }

  return result;
}

int magnitude(Items items) {
  while (items.size() > 1) {
    for (auto&& [i, item] : my_std::views::enumerate(items)) {
      if (i < items.size() - 1 and item.depth == items.at(i + 1).depth) {
        const int depth{item.depth};
        const int value{3 * item.value + 2 * items.at(i + 1).value};
        items.erase(items.begin() + i);
        items.erase(items.begin() + i);
        items.insert(items.begin() + i, Item{value, depth - 1});
        break;
      }
    }
  }
  if (items.empty()) {
    throw std::runtime_error("cannot compute magnitude of empty list");
  }
  return items.front().value;
}

auto find_part1(const Lines& ls) {
  return magnitude(ranges::fold_left(ls | views::drop(1), ls.front(), add));
}

auto find_part2(const Lines& ls) {
  const auto all_pairs{my_std::views::cartesian_product(ls, ls)};
  return ranges::max(views::transform(all_pairs, [](auto&& pair) {
    auto [line1, line2]{pair};
    return magnitude(add(line1, line2));
  }));
}

auto parse_lines(std::string_view path) {
  return aoc::slurp_lines(path) | views::transform([](std::string line) {
           std::vector<Item> items;
           int depth{};
           for (char ch : line) {
             switch (ch) {
               case '[': {
                 depth += 1;
               } break;
               case ']': {
                 depth -= 1;
               } break;
               case ',': {
               } break;
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9': {
                 items.emplace_back(ch - '0', depth);
               } break;
               default:
                 throw std::runtime_error(std::format("unknown character '{}'", ch));
             }
           }
           return items;
         })
         | ranges::to<std::vector>();
}

int main() {
  const Lines ls{parse_lines("/dev/stdin")};

  const auto part1{find_part1(ls)};
  const auto part2{find_part2(ls)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
