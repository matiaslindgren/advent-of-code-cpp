#include "aoc.hpp"
#include "std.hpp"

auto search(const auto& input) {
  int part1{};
  int part2{};

  const std::regex do_dont_or_mul(R"((do\(\)|don't\(\)|mul\((\d+),(\d+)\)))");
  bool doing{true};

  for (auto match_it{std::sregex_iterator(input.begin(), input.end(), do_dont_or_mul)};
       match_it != std::sregex_iterator();
       ++match_it) {
    const auto& match{*match_it};

    if (match.size() != 4) {
      throw std::runtime_error("invalid input, failed to match");
    }

    if (const auto instruction{match[0].str()}; instruction == "do()") {
      doing = true;
    } else if (instruction == "don't()") {
      doing = false;
    } else if (instruction.starts_with("mul(")) {
      const int a{std::stoi(match[2].str())};
      const int b{std::stoi(match[3].str())};
      part1 += a * b;
      part2 += int{doing} * (a * b);
    }
  }

  return std::pair{part1, part2};
}

int main() {
  const auto input{aoc::slurp_file("/dev/stdin")};
  const auto [part1, part2]{search(input)};
  std::println("{} {}", part1, part2);
  return 0;
}
