#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Instruction {
  enum class Type : unsigned char {
    rect,
    rotate_row,
    rotate_col,
  } type{};
  int a{};
  int b{};
};

std::istream& operator>>(std::istream& is, Instruction::Type& ins_type) {
  using Type = Instruction::Type;
  bool ok{false};
  if (std::string type; is >> type) {
    if (type == "rect") {
      ins_type = Type::rect;
      ok = true;
    } else if (type == "rotate" and is >> type) {
      if (type == "row") {
        ins_type = Type::rotate_row;
        ok = true;
      } else if (type == "column") {
        ins_type = Type::rotate_col;
        ok = true;
      }
    }
  }
  if (not is.eof() and not ok) {
    throw std::runtime_error("failed parsing Instruction::Type");
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  using Type = Instruction::Type;
  bool ok{false};
  if (Type type{}; is >> type) {
    switch (type) {
      case Type::rect: {
        if (int a{}, b{}; is >> a >> skip("x"s) >> b) {
          ins = {type, a, b};
          ok = true;
        }
      } break;
      case Type::rotate_row: {
        if (int y{}, by{}; is >> std::ws >> skip("y="s) >> y >> std::ws >> skip("by"s) >> by) {
          ins = {type, y, by};
          ok = true;
        }
      } break;
      case Type::rotate_col: {
        if (int x{}, by{}; is >> std::ws >> skip("x="s) >> x >> std::ws >> skip("by"s) >> by) {
          ins = {type, x, by};
          ok = true;
        }
      } break;
    }
    if (not ok) {
      throw std::runtime_error("failed parsing Instruction");
    }
  }
  return is;
}

static constexpr int width{50};
static constexpr int height{6};
using Screen = std::vector<bool>;

Screen run_instructions(const auto& instructions) {
  Screen screen(1UZ * width * height, false);
  for (const auto& ins : instructions) {
    switch (ins.type) {
      case Instruction::Type::rect: {
        for (int y{}; y < ins.b; ++y) {
          for (int x{}; x < ins.a; ++x) {
            screen.at(y * width + x) = true;
          }
        }
      } break;
      case Instruction::Type::rotate_row: {
        if (const int n{ins.b % width}; n) {
          const int y{ins.a};
          auto lhs{screen.begin() + 1UZ * y * width};
          auto rhs{lhs + width};
          auto mid{rhs - n};
          ranges::rotate(lhs, mid, rhs);
        }
      } break;
      case Instruction::Type::rotate_col: {
        if (const int n{ins.b % height}; n) {
          const int x{ins.a};
          auto columns{
              ranges::subrange(screen.begin() + x, screen.end()) | my_std::views::stride(width)
          };
          auto lhs{columns.begin()};
          auto rhs{lhs + height};
          auto mid{rhs - n};
          ranges::rotate(lhs, mid, rhs);
        }
      } break;
    }
  }
  return screen;
}

std::string decode_ascii(const Screen& screen) {
  constexpr auto char_height{6};
  constexpr auto char_width{5};
  // clang-format off
  return (
      views::iota(0, width)
      | my_std::views::stride(char_width)
      | views::transform([&](const auto& char_x_begin) {
          std::string ch;
          for (int y{}; y < char_height; ++y) {
            for (int x{}; x < char_width - 1; ++x) {
              const auto idx{y * width + char_x_begin + x};
              ch.push_back(screen[idx] ? '#' : '.');
            }
          }
          return aoc::ocr(ch);
        })
      | ranges::to<std::string>()
  );
  // clang-format on
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};
  const auto screen{run_instructions(instructions)};

  const auto part1{sum(screen)};
  const auto part2{decode_ascii(screen)};

  std::println("{} {}", part1, part2);

  return 0;
}
