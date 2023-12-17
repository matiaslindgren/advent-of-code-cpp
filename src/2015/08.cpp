import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

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

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

int main() {
  aoc::init_io();

  const auto lines{views::istream<std::string>(std::cin) | ranges::to<std::vector>()};

  const auto part1{sum(lines | views::transform(count_bytes))};
  const auto part2{sum(lines | views::transform(escape) | views::transform(count_bytes))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
