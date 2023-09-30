import std;

// TODO(llvm18)
namespace my_std {
namespace ranges {
// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_left
// (accessed 2023-09-30)
struct fold_left_fn {
  template <std::input_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U =
        std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
    if (first == last) return U(std::move(init));
    U accum = std::invoke(f, std::move(init), *first);
    for (++first; first != last; ++first)
      accum = std::invoke(f, std::move(accum), *first);
    return std::move(accum);
  }

  template <std::ranges::input_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r),
                   std::ranges::end(r),
                   std::move(init),
                   std::ref(f));
  }
};

inline constexpr fold_left_fn fold_left;
}  // namespace ranges
}  // namespace my_std

enum class State {
  Init,
  Literal,
  Escaping,
  Hexadecimal1,
  Hexadecimal2,
  End,
  Invalid,
};

std::ostream& operator<<(std::ostream& os, const State& state) {
  switch (state) {
    case State::Init: {
      os << "State::Init";
    } break;
    case State::Literal: {
      os << "State::Literal";
    } break;
    case State::Escaping: {
      os << "State::Escaping";
    } break;
    case State::Hexadecimal1: {
      os << "State::Hexadecimal1";
    } break;
    case State::Hexadecimal2: {
      os << "State::Hexadecimal2";
    } break;
    case State::End: {
      os << "State::End";
    } break;
    case State::Invalid: {
      os << "State::Invalid";
    } break;
  }
  return os;
}

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
            ++char_count;
            state = State::Hexadecimal1;
          } break;
        }
      } break;
      case State::Hexadecimal1: {
        state = State::Hexadecimal2;
      } break;
      case State::Hexadecimal2: {
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
  out << '"';
  for (auto ch : s) {
    switch (ch) {
      case '"': {
        out << '\\';
      } break;
      case '\\': {
        out << '\\';
      } break;
    }
    out << ch;
  }
  out << '"';
  return out.str();
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto lines = std::views::istream<std::string>(std::cin) |
                     std::ranges::to<std::vector<std::string>>();

  constexpr auto accumulate = std::bind(my_std::ranges::fold_left,
                                        std::placeholders::_1,
                                        0,
                                        std::plus<int>());

  const auto part1 = accumulate(lines | std::views::transform(count_bytes));
  const auto part2 = accumulate(lines | std::views::transform(escape) |
                                std::views::transform(count_bytes));
  std::print("{} {}\n", part1, part2);

  return 0;
}
