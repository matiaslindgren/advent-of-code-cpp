#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::skip;

struct Instruction {
  enum class Type : unsigned char {
    set_value,
    lo_bot_hi_bot,
    lo_out_hi_bot,
    lo_out_hi_out,
  } type{};
  int input{};
  int out1{};
  int out2{};
};

struct Gate {
  int out_lo{};
  int out_hi{};
  int input1{};
  int input2{};

  void set(int value) {
    if (input1 < 0) {
      input1 = value;
    } else {
      input2 = value;
    }
  }

  [[nodiscard]]
  auto value() const {
    return std::pair{std::min(input1, input2), std::max(input1, input2)};
  }

  [[nodiscard]]
  bool is_full() const {
    return std::min({out_lo, out_hi, input1, input2}) >= 0;
  }
};

constexpr auto product{std::__bind_back(ranges::fold_left, 1, std::multiplies{})};

auto search(const auto& instructions) {
  constexpr int mem_size{512};
  std::array<Gate, mem_size> mem{};
  mem.fill(Gate{-1, -1, -1, -1});

  auto bots{views::take(mem, mem_size / 2)};
  auto outputs{views::drop(mem, mem_size / 2)};

  for (const Instruction& ins : instructions) {
    switch (ins.type) {
      case Instruction::Type::set_value: {
        bots[ins.out1].set(ins.input);
      } break;
      case Instruction::Type::lo_bot_hi_bot: {
        auto& bot{bots[ins.input]};
        bot.out_lo = ins.out1;
        bot.out_hi = ins.out2;
      } break;
      case Instruction::Type::lo_out_hi_bot: {
        auto& bot{bots[ins.input]};
        bot.out_lo = mem_size / 2 + ins.out1;
        bot.out_hi = ins.out2;
      } break;
      case Instruction::Type::lo_out_hi_out: {
        auto& bot{bots[ins.input]};
        bot.out_lo = mem_size / 2 + ins.out1;
        bot.out_hi = mem_size / 2 + ins.out2;
      } break;
    }
  }

  for (auto full_bots{
           views::filter(bots, [](const auto& b) { return b.is_full(); })
           | ranges::to<std::vector>()
       };
       not full_bots.empty();) {
    const auto bot{full_bots.back()};
    full_bots.pop_back();

    auto& bot_lo{bots[bot.out_lo]};
    bot_lo.set(std::min(bot.input1, bot.input2));
    if (bot_lo.is_full()) {
      full_bots.push_back(bot_lo);
    }

    auto& bot_hi{bots[bot.out_hi]};
    bot_hi.set(std::max(bot.input1, bot.input2));
    if (bot_hi.is_full()) {
      full_bots.push_back(bot_hi);
    }
  }

  return std::pair{
      ranges::distance(
          bots.begin(),
          ranges::find_if(
              bots,
              [](const auto& b) {
                const auto [lo, hi]{b.value()};
                return lo == 17 and hi == 61;
              }
          )
      ),
      product(views::take(outputs, 3) | views::transform([](const auto& out) {
                return out.value().second;
              })),
  };
}

struct GiveBot {
  Instruction::Type type{};
  int out1{};
  int out2{};
};

std::istream& operator>>(std::istream& is, GiveBot& gb) {
  using Type = Instruction::Type;
  if (std::string cmd; is >> std::ws >> skip("gives low to"s) >> cmd) {
    if (cmd == "bot") {
      if (int out1{}, out2{}; is >> out1 >> std::ws >> skip("and high to bot"s) >> out2) {
        gb = {Type::lo_bot_hi_bot, out1, out2};
      }
    } else if (cmd == "output") {
      if (int out1{}, out2{}; is >> out1 >> std::ws >> skip("and high to"s) >> cmd >> out2) {
        if (cmd == "bot") {
          gb = {Type::lo_out_hi_bot, out1, out2};
        } else if (cmd == "output") {
          gb = {Type::lo_out_hi_out, out1, out2};
        }
      }
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  using Type = Instruction::Type;

  bool ok{false};
  if (auto [cmd, num]{std::tuple(""s, int{})}; is >> cmd >> num) {
    if (cmd == "value") {
      if (int bot{}; is >> std::ws >> skip("goes to bot"s) >> bot) {
        ins = {Type::set_value, num, bot};
        ok = true;
      }
    } else if (cmd == "bot") {
      if (GiveBot gb; is >> gb) {
        ins = {gb.type, num, gb.out1, gb.out2};
        ok = true;
      }
    }
  }
  if (not is.eof() and not ok) {
    throw std::runtime_error("failed parsing Instruction");
  }
  return is;
}

int main() {
  const auto instructions{aoc::parse_items<Instruction>("/dev/stdin")};
  const auto [part1, part2]{search(instructions)};
  std::println("{} {}", part1, part2);
  return 0;
}
