import std;
#include "tmp_util.hpp"

enum class State {
  Init,
  Literal,
  Escaping,
  Hexadecimal1,
  Hexadecimal2,
  End,
  Invalid,
};

auto count_bytes(const std::string& s) {
  decltype(s.size()) char_count{};
  auto state{State::Init};
  for (auto ch : s) {
    const auto prev_state{state};
    state = State::Invalid;
    switch (prev_state) {
      case State::Init: {
        switch (ch) {
          case '"': {
            state = State::Literal;
          } break;
        }
      } break;
      case State::Literal: {
        switch (ch) {
          case '\\': {
            state = State::Escaping;
          } break;
          case '"': {
            state = State::End;
          } break;
          default: {
            ++char_count;
            state = State::Literal;
          } break;
        }
      } break;
      case State::Escaping: {
        switch (ch) {
          case '\\':
          case '"': {
            ++char_count;
            state = State::Literal;
          } break;
          case 'x': {
            state = State::Hexadecimal1;
          } break;
        }
      } break;
      case State::Hexadecimal1: {
        state = State::Hexadecimal2;
      } break;
      case State::Hexadecimal2: {
        ++char_count;
        state = State::Literal;
      } break;
      case State::End:
      case State::Invalid: {
        throw std::runtime_error("invalid state while parsing line");
      }
    }
  }
  if (state != State::End) {
    throw std::runtime_error("invalid state after parsing line");
  }
  return s.size() - char_count;
}

auto escape(const std::string& s) {
  std::ostringstream out;
  out << std::quoted(s);
  return out.str();
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto lines
      = std::views::istream<std::string>(std::cin) | std::ranges::to<std::vector<std::string>>();

  constexpr auto accumulate
      = std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus<int>());

  const auto part1 = accumulate(lines | std::views::transform(count_bytes));
  const auto part2
      = accumulate(lines | std::views::transform(escape) | std::views::transform(count_bytes));
  std::print("{} {}\n", part1, part2);

  return 0;
}
