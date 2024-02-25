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

struct IntCode {
  using Int = long;

  std::vector<Int> memory;
  std::deque<Int> input, output;

  std::ptrdiff_t ip{}, relbase{};

  Int load(const auto i, Mode mode) {
    switch (mode) {
      case Mode::immediate: {
        return i;
      } break;
      case Mode::address: {
        return memory.at(i);
      } break;
      case Mode::relative_address: {
        return memory.at(i + relbase);
      } break;
    }
  }

  void store(const auto i, Int value, Mode mode) {
    switch (mode) {
      case Mode::immediate: {
        throw std::runtime_error("cannot store in immediate mode");
      } break;
      case Mode::address: {
        memory.at(i) = value;
      } break;
      case Mode::relative_address: {
        memory.at(i + relbase) = value;
      } break;
    }
  }

  Int pop_input() {
    if (not input.empty()) {
      auto res{input.front()};
      input.pop_front();
      return res;
    }
    throw std::runtime_error("cannot pop from empty input");
  }

  Int compute(Op op, Int lhs, Int rhs) const {
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
        break;
    }
    throw std::runtime_error(std::format("Op {} is not a binary operator", std::to_underlying(op)));
  }

  auto parse_instruction(auto ins) const {
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

  bool do_step() {
    auto&& [op, mode1, mode2, mode3]{parse_instruction(memory[ip++])};
    switch (op) {
      case Op::add:
      case Op::multiply:
      case Op::less:
      case Op::equal: {
        const Int lhs{load(memory[ip++], mode1)};
        const Int rhs{load(memory[ip++], mode2)};
        const Int out{compute(op, lhs, rhs)};
        store(memory[ip++], out, mode3);
      } break;
      case Op::input: {
        store(memory[ip++], pop_input(), mode1);
      } break;
      case Op::output: {
        output.push_back(load(memory[ip++], mode1));
      } break;
      case Op::jump_if_nonzero:
      case Op::jump_if_zero: {
        const Int flag{load(memory[ip++], mode1)};
        if ((flag != 0) == (op == Op::jump_if_nonzero)) {
          ip = load(memory[ip], mode2);
        } else {
          ip += 1;
        }
      } break;
      case Op::update_rel_base: {
      } break;
      case Op::end: {
        return false;
      } break;
    }
    return true;
  }

  void run() {
    while (do_step()) {
    }
  }

  void dump_memory(std::ostream& os) const {
    std::ranges::copy(memory, std::ostream_iterator<Int>(os, " "));
    os << "\n";
  }
};

std::vector<IntCode::Int> parse_input(std::string input) {
  std::ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  auto inputs{std::views::istream<IntCode::Int>(is) | std::ranges::to<std::vector>()};
  if (is.eof()) {
    return inputs;
  }
  throw std::runtime_error("invalid IntCode input, parsing failed");
}

}  // namespace intcode
