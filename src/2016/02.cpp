import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Step : char {
  up = 'U',
  left = 'L',
  down = 'D',
  right = 'R',
};

std::istream& operator>>(std::istream& is, Step& step) {
  if (std::underlying_type_t<Step> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Step::up):
      case std::to_underlying(Step::left):
      case std::to_underlying(Step::down):
      case std::to_underlying(Step::right):
        step = {ch};
        return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Step");
}

using Steps = std::vector<Step>;

std::istream& operator>>(std::istream& is, Steps& steps) {
  if (std::string line; is >> line) {
    std::stringstream line_stream{line};
    steps = {views::istream<Step>(line_stream) | ranges::to<Steps>()};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Steps");
}

using Keypad = std::vector<std::string>;

std::pair<unsigned, unsigned> find(const Keypad& keypad, char key) {
  for (unsigned y{}; y < keypad.size(); ++y) {
    for (unsigned x{}; x < keypad.size(); ++x) {
      if (keypad[y][x] == key) {
        return {y, x};
      }
    }
  }
  throw std::runtime_error(std::format("keypad has no {}", key));
}

std::string find_code(const Keypad& keypad, const std::vector<Steps>& instructions) {
  auto [y, x] = find(keypad, '5');
  return (
      views::transform(
          instructions,
          [&](const auto& steps) mutable -> char {
            for (const auto& step : steps) {
              unsigned y2{y}, x2{x};
              switch (step) {
                case Step::up: {
                  y2 = y - 1;
                } break;
                case Step::left: {
                  x2 = x - 1;
                } break;
                case Step::down: {
                  y2 = y + 1;
                } break;
                case Step::right: {
                  x2 = x + 1;
                } break;
              }
              if (keypad[y2][x2]) {
                y = y2;
                x = x2;
              }
            }
            return keypad[y][x];
          }
      )
      | ranges::to<std::string>()
  );
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto instructions{views::istream<Steps>(input) | ranges::to<std::vector>()};

  const Keypad keypad1{{
      {'\0', '\0', '\0', '\0', '\0'},
      {'\0', '1', '2', '3', '\0'},
      {'\0', '4', '5', '6', '\0'},
      {'\0', '7', '8', '9', '\0'},
      {'\0', '\0', '\0', '\0', '\0'},
  }};
  const auto part1{find_code(keypad1, instructions)};

  const Keypad keypad2{{
      {'\0', '\0', '\0', '\0', '\0', '\0', '\0'},
      {'\0', '\0', '\0', '1', '\0', '\0', '\0'},
      {'\0', '\0', '2', '3', '4', '\0', '\0'},
      {'\0', '5', '6', '7', '8', '9', '\0'},
      {'\0', '\0', 'A', 'B', 'C', '\0', '\0'},
      {'\0', '\0', '\0', 'D', '\0', '\0', '\0'},
      {'\0', '\0', '\0', '\0', '\0', '\0', '\0'},
  }};
  const auto part2{find_code(keypad2, instructions)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
