module;
import std;

export module intcode;

export namespace intcode {

enum class Op : int {
  add = 1,
  multiply = 2,
  input = 3,
  output = 4,
  jump_true = 5,
  jump_false = 6,
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
    case std::to_underlying(Op::jump_true):
    case std::to_underlying(Op::jump_false):
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
  literal = 1,
  relative_address = 2,
};

struct IntCode {
  using Int = long;

  std::vector<Int> memory, input, output;

  std::size_t ip{};
  std::ptrdiff_t relbase{};

  Int load(const auto i, Mode mode) {
    switch (mode) {
      case Mode::literal: {
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
      case Mode::literal: {
        throw std::runtime_error("cannot write to a literal");
      } break;
      case Mode::address: {
        memory.at(i) = value;
      } break;
      case Mode::relative_address: {
        memory.at(i + relbase) = value;
      } break;
    }
  }

  Int compute(Op op, Int lhs, Int rhs) const {
    switch (op) {
      case Op::add:
        return lhs + rhs;
      case Op::multiply:
        return lhs * rhs;
      default:
        break;
    }
    throw std::runtime_error(std::format("Op {} is not a binary operator", std::to_underlying(op)));
  }

  bool do_step() {
    const Op op{int2op(load(memory[ip++], Mode::literal))};
    switch (op) {
      case Op::add:
      case Op::multiply: {
        const Int lhs{load(memory[ip++], Mode::address)};
        const Int rhs{load(memory[ip++], Mode::address)};
        const Int out{compute(op, lhs, rhs)};
        store(memory[ip++], out, Mode::address);
      } break;
      case Op::input: {
      } break;
      case Op::output: {
      } break;
      case Op::jump_true: {
      } break;
      case Op::jump_false: {
      } break;
      case Op::less: {
      } break;
      case Op::equal: {
      } break;
      case Op::update_rel_base: {
      } break;
      case Op::end: {
        return false;
      } break;
    }
    return true;
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
