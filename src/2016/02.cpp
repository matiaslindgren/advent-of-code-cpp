import std;

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

std::pair<unsigned, unsigned> find_yx_of_5(const Keypad& keypad) {
  for (unsigned y{}; y < keypad.size(); ++y) {
    for (unsigned x{}; x < keypad.size(); ++x) {
      if (keypad[y][x] == '5') {
        return {y, x};
      }
    }
  }
  throw std::runtime_error("keypad has no 5");
}

std::string find_code(const Keypad& keypad, const std::vector<Steps>& instructions) {
  std::string code;
  auto [y, x] = find_yx_of_5(keypad);
  for (const auto& steps : instructions) {
    for (const auto& step : steps) {
      unsigned y2{y}, x2{x};
      switch (step) {
        case Step::up: {
          y2 = std::min(y, y - 1);
        } break;
        case Step::left: {
          x2 = std::min(x, x - 1);
        } break;
        case Step::down: {
          y2 = std::min(keypad.size(), y + 1uz);
        } break;
        case Step::right: {
          x2 = std::min(keypad.size(), x + 1uz);
        } break;
      }
      if (keypad[y2][x2]) {
        y = y2;
        x = x2;
      }
    }
    code.push_back(keypad[y][x]);
  }
  return code;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto instructions = views::istream<Steps>(std::cin) | ranges::to<std::vector<Steps>>();

  const Keypad keypad1 = {
      "123",
      "456",
      "789",
  };
  const auto part1{find_code(keypad1, instructions)};

  const Keypad keypad2 = {
      {  0,   0, '1',   0,   0},
      {  0, '2', '3', '4',   0},
      {'5', '6', '7', '8', '9'},
      {  0, 'A', 'B', 'C',   0},
      {  0,   0, 'D',   0,   0},
  };
  const auto part2{find_code(keypad2, instructions)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
