import std;

std::pair<int, int> parse_garbage(std::string_view s) {
  enum class State {
    group,
    garbage,
    cancel,
    comma,
    invalid,
  };

  int depth{}, score{}, garbage_count{};

  auto state{State::group};

  for (char ch : s) {
    switch (std::exchange(state, State::invalid)) {
      case State::group: {
        switch (ch) {
          case '{': {
            depth += 1;
            state = State::group;
          } break;
          case '}': {
            score += depth;
            depth -= 1;
            state = State::group;
          } break;
          case '<': {
            state = State::garbage;
          } break;
          case ',': {
            state = State::comma;
          } break;
        }
      } break;

      case State::garbage: {
        switch (ch) {
          case '!': {
            state = State::cancel;
          } break;
          case '>': {
            state = State::group;
          } break;
          default: {
            garbage_count += 1;
            state = State::garbage;
          } break;
        }
      } break;

      case State::cancel: {
        state = State::garbage;
      } break;

      case State::comma: {
        switch (ch) {
          case '{': {
            depth += 1;
            state = State::group;
          } break;
          case '<': {
            state = State::garbage;
          } break;
        }
      } break;

      case State::invalid: {
        throw std::runtime_error("invalid state during parsing");
      }
    }
  }

  if (depth != 0 || state != State::group) {
    throw std::runtime_error("invalid final state");
  }

  return {score, garbage_count};
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string input;
  std::cin >> input;

  const auto [part1, part2] = parse_garbage(input);
  std::print("{} {}\n", part1, part2);

  return 0;
}
