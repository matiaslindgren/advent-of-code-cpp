#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

int item_priority(char ch) {
  if ('a' <= ch and ch <= 'z') {
    return ch - 'a' + 1;
  }
  if ('A' <= ch and ch <= 'Z') {
    return ch - 'A' + 27;
  }
  throw std::runtime_error(std::format("unknown item '{}', expected [a-zA-Z]", ch));
}

auto find_part1(const auto& lines) {
  return sum(views::transform(lines, [](auto line) {
    std::set<char> common;
    auto lhs{line.substr(0, line.size() / 2) | ranges::to<std::set>()};
    auto rhs{line.substr(line.size() / 2) | ranges::to<std::set>()};
    ranges::set_intersection(lhs, rhs, std::inserter(common, common.end()));
    if (common.empty()) {
      throw std::runtime_error(std::format("no characters in common on line '{}'", line));
    }
    return item_priority(*common.begin());
  }));
}

// TODO libc++19?
auto chunks3(auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2)) | my_std::views::stride(3);
}

auto find_part2(const auto& lines) {
  return sum(
      chunks3(views::transform(lines, [](auto line) { return ranges::to<std::set>(line); }))
      | views::transform([](auto&& bags) {
          auto [bag1, bag2, bag3]{bags};
          std::set<char> common12, common13, common123;
          ranges::set_intersection(bag1, bag2, std::inserter(common12, common12.end()));
          ranges::set_intersection(bag1, bag3, std::inserter(common13, common13.end()));
          ranges::set_intersection(common12, common13, std::inserter(common123, common123.end()));
          if (common123.empty()) {
            throw std::runtime_error("3 bags with no items in common");
          }
          return item_priority(*common123.begin());
        })
  );
}

int main() {
  const auto lines{aoc::slurp_lines("/dev/stdin")};

  if (lines.size() % 3 != 0) {
    throw std::runtime_error("amount of lines must be divisible by 3");
  }
  if (ranges::any_of(lines, [](auto s) { return s.empty() or s.size() % 2; })) {
    throw std::runtime_error("every line must be of even length");
  }

  const auto part1{find_part1(lines)};
  const auto part2{find_part2(lines)};

  std::println("{} {}", part1, part2);

  return 0;
}
