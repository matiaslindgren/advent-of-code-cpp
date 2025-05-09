#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

enum class State : unsigned char {
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
    switch (std::exchange(state, State::Invalid)) {
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

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  const auto lines{aoc::parse_items<std::string>("/dev/stdin")};

  const auto part1{sum(lines | views::transform(count_bytes))};
  const auto part2{sum(lines | views::transform(escape) | views::transform(count_bytes))};

  std::println("{} {}", part1, part2);

  return 0;
}
