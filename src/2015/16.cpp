#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Aunt {
  std::size_t id{};
  std::unordered_map<std::string, int> items;
};

static const std::unordered_map target_items{
    std::pair{"children"s, 3},
    std::pair{"cats"s, 7},
    std::pair{"samoyeds"s, 2},
    std::pair{"pomeranians"s, 3},
    std::pair{"akitas"s, 0},
    std::pair{"vizslas"s, 0},
    std::pair{"goldfish"s, 5},
    std::pair{"trees"s, 3},
    std::pair{"cars"s, 2},
    std::pair{"perfumes"s, 1}
};

Aunt find_target_aunt(std::vector<Aunt> aunts, auto comp) {
  for (const auto& [k, v] : target_items) {
    const auto is_mismatch{[&](const auto& aunt) {
      return aunt.items.contains(k) and not comp(k, aunt.items.at(k), v);
    }};
    const auto [rm_begin, rm_end] = ranges::remove_if(aunts, is_mismatch);
    aunts.erase(rm_begin, rm_end);
  }
  return aunts.front();
}

std::istream& operator>>(std::istream& is, Aunt& aunt) {
  if (Aunt a; is >> std::ws >> skip("Sue"s) >> a.id >> std::ws >> skip(":"s)) {
    for (auto [key, val]{std::pair{""s, int{}}}; is >> key and key.ends_with(":") and is >> val;) {
      key.pop_back();
      a.items[key] = val;
      if (is.peek() != ',' or is.get() == 0) {
        break;
      }
    }
    if (is or is.eof()) {
      aunt = a;
    }
  }
  return is;
}

int main() {
  const auto aunts{aoc::parse_items<Aunt>("/dev/stdin")};

  const auto equal_compare{[](const auto&, int lhs, int rhs) { return lhs == rhs; }};
  const auto fancy_compare{[](const auto& key, int lhs, int rhs) {
    if (key == "cats" or key == "trees") {
      return lhs > rhs;
    }
    if (key == "pomerians" or key == "goldfish") {
      return lhs < rhs;
    }
    return lhs == rhs;
  }};

  const auto aunt1{find_target_aunt(aunts, equal_compare)};
  const auto aunt2{find_target_aunt(aunts, fancy_compare)};

  std::println("{} {}", aunt1.id, aunt2.id);

  return 0;
}
