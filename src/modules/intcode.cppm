module;
import std;

export module intcode;

export namespace intcode {

enum class Op : int {
  add = 1,
  multiply = 2,
  input = 3,
  output = 4,
  jump_if_nonzero = 5,
  jump_if_zero = 6,
  less = 7,
  equal = 8,
  update_rel_base = 9,
  end = 99,
};

Op int2op(int x) {
  using Op = intcode::Op;
  switch (x) {
    case std::to_underlying(Op::add):
    case std::to_underlying(Op::multiply):
    case std::to_underlying(Op::input):
    case std::to_underlying(Op::output):
    case std::to_underlying(Op::jump_if_nonzero):
    case std::to_underlying(Op::jump_if_zero):
    case std::to_underlying(Op::less):
    case std::to_underlying(Op::equal):
    case std::to_underlying(Op::update_rel_base):
    case std::to_underlying(Op::end):
      return Op{x};
  }
  throw std::runtime_error(std::format("no intcode::Op for {}", x));
}

enum class Mode : int {
  address = 0,
  immediate = 1,
  relative_address = 2,
};

class IntCode {
 public:
  using Int = long;

 private:
  std::unordered_map<Int, Int> memory;
  std::deque<Int> input;
  std::optional<Int> output;

  Int ip{}, relbase{};

  [[nodiscard]] auto parse_instruction(auto ins) const noexcept {
    std::array<int, 5> d;
    for (int& x : d) {
      auto&& [q, r]{std::ldiv(ins, 10)};
      ins = q;
      x = r;
    }
    return std::tuple{
        int2op(d[0] + d[1] * 10),
        Mode{d[2]},
        Mode{d[3]},
        Mode{d[4]},
    };
  }

  [[nodiscard]] Int next() {
    if (0 <= ip) {
      return load(ip++, Mode::address);
    }
    throw std::runtime_error("instruction pointer is out of bounds");
  }

 public:
  IntCode() = default;
  IntCode(std::ranges::input_range auto&& program) {
    for (auto&& [i, value] : std::views::zip(std::views::iota(0uz), program)) {
      memory[i] = value;
    }
  }

  [[nodiscard]] Int load(const auto i, const Mode mode) const {
    switch (mode) {
      case Mode::immediate:
        return i;
      case Mode::address:
        return memory.contains(i) ? memory.at(i) : 0;
      case Mode::relative_address:
        return load(i + relbase, Mode::address);
    }
  }

  Int& store(const auto i, Int value, const Mode mode) {
    switch (mode) {
      case Mode::immediate:
        throw std::runtime_error("cannot store in immediate mode");
      case Mode::address:
        return (memory[i] = value);
      case Mode::relative_address:
        return store(i + relbase, value, Mode::address);
    }
  }

  void push_input(std::ranges::view auto&& v) {
    input.append_range(v);
  }

  template <typename T>
    requires(not std::ranges::range<T>)
  void push_input(T value) {
    input.push_back(value);
  }

  Int pop_input() {
    if (not input.empty()) {
      auto res{input.front()};
      input.pop_front();
      return res;
    }
    throw std::runtime_error("cannot pop from empty input queue");
  }

  auto pop_output() {
    return std::exchange(output, std::nullopt);
  }

  [[nodiscard]] Int compute(const Op op, const Int lhs, const Int rhs) const {
    switch (op) {
      case Op::add:
        return lhs + rhs;
      case Op::multiply:
        return lhs * rhs;
      case Op::less:
        return lhs < rhs;
      case Op::equal:
        return lhs == rhs;
      default:
        throw std::runtime_error(
            std::format("Op {} is not a binary operator", std::to_underlying(op))
        );
    }
  }

  void do_step() {
    auto&& [op, mode1, mode2, mode3]{parse_instruction(next())};
    switch (op) {
      case Op::add:
      case Op::multiply:
      case Op::less:
      case Op::equal: {
        const Int lhs{load(next(), mode1)};
        const Int rhs{load(next(), mode2)};
        const Int out{compute(op, lhs, rhs)};
        store(next(), out, mode3);
      } break;
      case Op::input: {
        store(next(), pop_input(), mode1);
      } break;
      case Op::output: {
        output = load(next(), mode1);
      } break;
      case Op::jump_if_nonzero:
      case Op::jump_if_zero: {
        const Int flag{load(next(), mode1)};
        const Int jump{load(next(), mode2)};
        if ((flag != 0) == (op == Op::jump_if_nonzero)) {
          ip = jump;
        }
      } break;
      case Op::update_rel_base: {
        relbase += load(next(), mode1);
      } break;
      case Op::end: {
        ip = std::numeric_limits<Int>::min();
      } break;
    }
  }

  [[nodiscard]] bool is_done() const noexcept {
    return ip < 0;
  }

  void run_to_end() {
    while (not is_done()) {
      do_step();
    }
  }

  void run_until_output() {
    while (not(is_done() or output)) {
      do_step();
    }
  }
};

std::vector<IntCode::Int> parse_program(std::string input) {
  std::ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  auto program{std::views::istream<IntCode::Int>(is) | std::ranges::to<std::vector>()};
  if (is.eof()) {
    return program;
  }
  throw std::runtime_error("invalid IntCode program, parsing failed");
}

}  // namespace intcode
