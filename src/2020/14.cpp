#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using std::operator""sv;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Bit : char {
  zero = '0',
  one = '1',
  floating = 'X',
};

struct Instruction {
  enum {
    new_mask,
    write,
  } type;
  std::vector<Bit> mask{};
  long dst{}, src{};
};

std::istream& operator>>(std::istream& is, Bit& bit) {
  if (std::underlying_type_t<Bit> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Bit::zero):
      case std::to_underlying(Bit::one):
      case std::to_underlying(Bit::floating): {
        bit = {ch};
        return is;
      } break;
      default: {
        throw std::runtime_error(std::format("unknown bit {}", ch));
      } break;
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    for (char rm : "=[]"sv) {
      ranges::replace(line, rm, ' ');
    }
    std::istringstream ls{line};
    if (std::string type; ls >> type) {
      if (type == "mask") {
        if (std::string mask; ls >> mask and not mask.empty()) {
          std::istringstream bs{mask};
          ins = {
              .type = Instruction::new_mask,
              .mask = views::istream<Bit>(bs) | ranges::to<std::vector>(),
          };
        }
      } else if (type == "mem") {
        if (long dst, src; ls >> dst >> src) {
          ins = {.type = Instruction::write, .dst = dst, .src = src};
        }
      } else {
        throw std::runtime_error(std::format("type must be mask or mem, not {}", type));
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

auto find_part1(const auto& instructions) {
  std::unordered_map<long, long> mem;
  {
    std::vector<Bit> mask;
    for (const auto& ins : instructions) {
      switch (ins.type) {
        case Instruction::new_mask: {
          mask = ins.mask;
        } break;
        case Instruction::write: {
          mem[ins.dst] = ranges::fold_left(
              views::iota(0uz, mask.size()),
              ins.src,
              [&mask](auto res, auto i) -> long {
                auto bit{1L << (mask.size() - i - 1)};
                switch (mask[i]) {
                  case Bit::zero:
                    return res & ~bit;
                  case Bit::one:
                    return res | bit;
                  case Bit::floating:
                    return res;
                }
              }
          );
        } break;
      }
    }
  }
  return sum(views::values(mem));
}

auto indexes_from_mask(const auto& mask, long idx1) {
  std::vector<long> combinations{0};
  auto idx2{0L};
  for (auto [i, b] : my_std::views::enumerate(mask)) {
    auto bit{1L << (mask.size() - i - 1)};
    switch (b) {
      case Bit::zero: {
        idx2 |= idx1 & bit;
      } break;
      case Bit::one: {
        idx2 |= bit;
      } break;
      case Bit::floating: {
        for (auto j : std::exchange(combinations, {})) {
          combinations.append_range(std::array{j, j | bit});
        }
      } break;
    }
  }
  return views::transform(combinations, [&idx2](auto i) { return i | idx2; })
         | ranges::to<std::vector>();
}

auto find_part2(const auto& instructions) {
  std::unordered_map<long, long> mem;
  {
    std::vector<Bit> mask;
    for (const auto& ins : instructions) {
      switch (ins.type) {
        case Instruction::new_mask: {
          mask = ins.mask;
        } break;
        case Instruction::write: {
          for (auto idx : indexes_from_mask(mask, ins.dst)) {
            mem[idx] = ins.src;
          }
        } break;
      }
    }
  }
  return sum(views::values(mem));
}

int main() {
  const auto instructions{aoc::slurp<Instruction>("/dev/stdin")};

  const auto part1{find_part1(instructions)};
  const auto part2{find_part2(instructions)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
