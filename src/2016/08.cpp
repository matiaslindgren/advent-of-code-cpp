import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Instruction {
  enum class Type {
    rect,
    rotate_row,
    rotate_col,
  } type;
  int a;
  int b;
};

std::istream& operator>>(std::istream& is, Instruction::Type& ins_type) {
  using Type = Instruction::Type;
  if (std::string type; is >> type) {
    if (type == "rect") {
      ins_type = Type::rect;
      return is;
    }
    if (type == "rotate" and is >> type) {
      if (type == "row") {
        ins_type = Type::rotate_row;
        return is;
      }
      if (type == "column") {
        ins_type = Type::rotate_col;
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction::Type");
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  using Type = Instruction::Type;
  if (Type type; is >> type) {
    switch (type) {
      case Type::rect: {
        if (int a, b; is >> a and is.ignore(1, 'x') and is >> b) {
          ins = {type, a, b};
          return is;
        }
      } break;
      case Type::rotate_row:
      case Type::rotate_col: {
        if (is.ignore(3, '=')) {
          if (int a, b; is >> a and is.ignore(3, 'y') and is >> b) {
            ins = {type, a, b};
            return is;
          }
        }
      } break;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

static constexpr auto width{50};
static constexpr auto height{6};
using Screen = std::vector<bool>;

Screen run_instructions(const auto& instructions) {
  Screen screen(width * height, false);
  for (const auto& ins : instructions) {
    switch (ins.type) {
      case Instruction::Type::rect: {
        for (int y{}; y < ins.b; ++y) {
          for (int x{}; x < ins.a; ++x) {
            screen[y * width + x] = true;
          }
        }
      } break;
      case Instruction::Type::rotate_row: {
        if (const int n{ins.b % width}; n) {
          const int y{ins.a};
          auto lhs{screen.begin() + (y * width)};
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

int count_on(const Screen& screen) {
  return ranges::count_if(screen, std::identity{});
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
          return aoc::as_ascii(ch);
        })
      | ranges::to<std::string>()
  );
  // clang-format on
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto instructions{views::istream<Instruction>(input) | ranges::to<std::vector>()};

  const auto screen{run_instructions(instructions)};

  const auto part1{count_on(screen)};
  const auto part2{decode_ascii(screen)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
