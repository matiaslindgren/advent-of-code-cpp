#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

constexpr int n_rows{6};
constexpr int n_letters{8};
constexpr int n_columns{40};
constexpr int letter_width{4};

std::string ocr(std::string image) {
  std::string word;
  for (int l{}; l < n_letters; ++l) {
    const auto n_spaces{l * (letter_width + 1)};
    std::string letter;
    for (int y{}; y < n_rows; ++y) {
      for (int x{}; x < letter_width; ++x) {
        letter.push_back(image.at(y * n_columns + x + n_spaces));
      }
    }
    word.push_back(aoc::ocr(letter));
  }
  return word;
}

auto search(const auto& adds) {
  int part1{};
  std::string image;
  {
    int register_x{1};
    int cycles{};
    for (int cycle{1}, lineno{0}; lineno < adds.size(); ++cycle) {
      auto op_value{adds.at(lineno)};
      if (not cycles) {
        cycles = 1 + (op_value != 0);
      }
      if (cycle % n_columns == 20) {
        part1 += register_x * cycle;
      }

      auto draw_pos{(cycle - 1) % n_columns};
      image.push_back(std::abs(draw_pos - register_x) < 2 ? '#' : '.');

      if (--cycles == 0) {
        register_x += op_value;
        ++lineno;
      }
    }
  }
  return std::pair{part1, ocr(image)};
}

auto parse_instructions(std::string_view path) {
  std::vector<int> adds;
  {
    const auto lines{aoc::slurp_lines(path)};
    for (auto line : lines) {
      std::istringstream ls{line};
      if (std::string cmd; ls >> cmd) {
        if (int n; cmd == "addx"s and ls >> n) {
          adds.push_back(n);
        } else if (cmd == "noop"s) {
          adds.push_back(0);
        } else {
          ls.setstate(std::ios_base::failbit);
        }
      }
      if (ls.fail() or not ls.eof()) {
        throw std::runtime_error(std::format("failed parsing line '{}'", line));
      }
    }
  }
  return adds;
}

int main() {
  const auto adds{parse_instructions("/dev/stdin")};
  const auto [part1, part2]{search(adds)};
  std::println("{} {}", part1, part2);
  return 0;
}
