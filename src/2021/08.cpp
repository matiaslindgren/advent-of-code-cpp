#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using Strings = std::vector<std::string>;

constexpr auto digit_map{"4725360918"s};
constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

struct Entry {
  Strings inputs;
  Strings output;

  [[nodiscard]]
  auto decode() const {
    // idea from
    // https://www.reddit.com/r/adventofcode/comments/rbj87a/comment/hnp4saz
    // accessed 2024-05-01
    std::unordered_map<char, int> input_counts;
    for (char ch : inputs | views::join) {
      input_counts[ch] += 1;
    }
    return output | views::transform([&](const std::string& s) {
             return sum(views::transform(s, [&](char ch) { return input_counts[ch]; }));
           })
           | views::transform([&](int count) {
               auto idx{((count / 2) % 15) % 11};
               return digit_map.at(idx);
             })
           | ranges::to<std::string>();
  }
};

auto search(const auto& entries) {
  int part1{};
  int part2{};
  for (const Entry& e : entries) {
    const auto output{e.decode()};
    for (char ch : output) {
      part1 += "1478"s.contains(ch);
    }
    part2 += std::stoi(output);
  }
  return std::pair{part1, part2};
}

std::istream& operator>>(std::istream& is, Entry& entry) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    Strings inputs;
    for (std::string item; ls >> item and item != "|"s;) {
      inputs.push_back(item);
    }
    Strings output;
    for (std::string item; ls >> item;) {
      output.push_back(item);
    }
    if (ls.eof() and not inputs.empty() and not output.empty()) {
      entry = {inputs, output};
    } else {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

int main() {
  const auto entries{aoc::parse_items<Entry>("/dev/stdin")};
  const auto [part1, part2]{search(entries)};
  std::println("{} {}", part1, part2);
  return 0;
}
