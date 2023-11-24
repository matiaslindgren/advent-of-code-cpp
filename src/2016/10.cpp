import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Instruction {
  enum class Type {
    set_value,
    lo_bot_hi_bot,
    lo_out_hi_bot,
    lo_out_hi_out,
  } type;
  int input;
  int out1;
  int out2;
};

std::istream& operator>>(std::istream& is, Instruction& ins) {
  using std::operator""s;
  using Type = Instruction::Type;
  using aoc::skip;

  if (auto [cmd, num] = std::tuple(std::string{}, int{}); is >> cmd >> num) {
    if (cmd == "value") {
      if (int bot; skip(is, " goes to bot"s) && is >> bot) {
        ins = {Type::set_value, num, bot};
        return is;
      }
    }
    if (cmd == "bot" && skip(is, " gives low to"s) && is >> cmd) {
      if (cmd == "bot") {
        if (int out1, out2; is >> out1 && skip(is, " and high to bot"s) && is >> out2) {
          ins = {Type::lo_bot_hi_bot, num, out1, out2};
          return is;
        }
      }
      if (cmd == "output") {
        if (int out1, out2; is >> out1 && skip(is, " and high to"s) && is >> cmd >> out2) {
          if (cmd == "bot") {
            ins = {Type::lo_out_hi_bot, num, out1, out2};
            return is;
          }
          if (cmd == "output") {
            ins = {Type::lo_out_hi_out, num, out1, out2};
            return is;
          }
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

struct Gate {
  int out_lo;
  int out_hi;
  int input1;
  int input2;
  constexpr void set(int value) {
    if (input1 < 0) {
      input1 = value;
    } else {
      input2 = value;
    }
  }
  constexpr std::pair<int, int> value() const {
    return {std::min(input1, input2), std::max(input1, input2)};
  }
  constexpr bool is_full() const {
    return !(out_lo < 0 || out_hi < 0 || input1 < 0 || input2 < 0);
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);

  std::array<Gate, 512> mem;
  mem.fill({-1, -1, -1, -1});

  const auto bots{ranges::subrange(mem.begin(), mem.begin() + 256)};
  const auto outputs{ranges::subrange(mem.begin() + 256, mem.end())};

  for (Instruction ins : views::istream<Instruction>(std::cin)) {
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
        bot.out_lo = 256 + ins.out1;
        bot.out_hi = ins.out2;
      } break;
      case Instruction::Type::lo_out_hi_out: {
        auto& bot{bots[ins.input]};
        bot.out_lo = 256 + ins.out1;
        bot.out_hi = 256 + ins.out2;
      } break;
    }
  }

  for (auto full_bots{
           views::filter(bots, [](auto&& b) { return b.is_full(); })
           | ranges::to<std::vector<Gate>>()};
       !full_bots.empty();) {
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

  const auto part1{ranges::distance(bots.begin(), ranges::find_if(bots, [](auto&& b) {
                                      const auto [lo, hi] = b.value();
                                      return lo == 17 && hi == 61;
                                    }))};
  const auto part2{my_std::ranges::fold_left(
      ranges::subrange(outputs.begin(), outputs.begin() + 3)
          | views::transform([](auto&& out) { return out.value().second; }),
      1,
      std::multiplies{}
  )};
  std::print("{} {}\n", part1, part2);

  return 0;
}
