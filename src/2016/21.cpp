#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::skip;

struct Operation {
  enum Type : unsigned char {
    swap_index,
    swap_value,
    rotate_index_of,
    rotate_index_of_inverse,
    rotate_left,
    rotate_right,
    reverse,
    move,
  } type{};
  unsigned idx0{};
  unsigned idx1{};
  char val0{};
  char val1{};
};

std::string scramble(std::string s, Operation op) {
  if (std::max(op.idx0, op.idx1) >= s.size()) {
    throw std::runtime_error("operation has an index which exceeds input string size");
  }
  using Type = Operation::Type;
  switch (op.type) {
    case Type::swap_value:
      op.idx0 = s.find(op.val0);
      op.idx1 = s.find(op.val1);
      [[fallthrough]];
    case Type::swap_index:
      std::swap(s[op.idx0], s[op.idx1]);
      break;
    case Type::rotate_index_of_inverse: {
      auto i{s.find(op.val0)};
      if (i == 0) {
        i = s.size();
      }
      op.idx0 = 1 + (i / 2) + 4L * int{i % 2 == 0};
      [[fallthrough]];
    }
    case Type::rotate_left:
      ranges::rotate(s, s.begin() + (op.idx0 % ranges::ssize(s)));
      break;
    case Type::rotate_index_of: {
      auto i{s.find(op.val0)};
      op.idx0 = 1 + i + int{i > 3};
      [[fallthrough]];
    }
    case Type::rotate_right:
      ranges::rotate(s, s.end() - (op.idx0 % ranges::ssize(s)));
      break;
    case Type::reverse:
      ranges::reverse(s.begin() + op.idx0, s.begin() + op.idx1 + 1);
      break;
    case Type::move: {
      const auto it0{s.begin() + op.idx0};
      const auto it1{s.begin() + op.idx1};
      if (op.idx0 < op.idx1) {
        ranges::rotate(it0, it0 + 1, it1 + 1);
      } else if (op.idx0 > op.idx1) {
        ranges::rotate(it1, it0, it0 + 1);
      }
    } break;
  }
  return s;
}

auto invert(const auto& operations) {
  return views::transform(operations | views::reverse, [](Operation op) {
    using Type = Operation::Type;
    switch (op.type) {
      case Type::rotate_left:
        return Operation{.type = Type::rotate_right, .idx0 = op.idx0};
      case Type::rotate_right:
        return Operation{.type = Type::rotate_left, .idx0 = op.idx0};
      case Type::rotate_index_of:
        return Operation{.type = Type::rotate_index_of_inverse, .val0 = op.val0};
      case Type::move:
        return Operation{.type = op.type, .idx0 = op.idx1, .idx1 = op.idx0};
      default:
        return op;
    }
  });
}

struct SwapOp {
  Operation::Type type{};
  unsigned lhs{};
  unsigned rhs{};
  char a{};
  char b{};
};

std::istream& operator>>(std::istream& is, SwapOp& op) {
  if (std::string type; is >> type) {
    if (unsigned lhs{}, rhs{};
        type == "position" and is >> lhs >> std::ws >> skip("with position"s) >> rhs) {
      op = {.type = Operation::Type::swap_index, .lhs = lhs, .rhs = rhs};
    } else if (char a{}, b{};
               type == "letter" and is >> a >> std::ws >> skip("with letter"s) >> b) {
      op = {.type = Operation::Type::swap_value, .a = a, .b = b};
    }
  }
  return is;
}

struct RotateOp {
  Operation::Type type{};
  char letter{};
  unsigned steps{};
};

std::istream& operator>>(std::istream& is, RotateOp& op) {
  if (std::string type; is >> type) {
    if (char letter{};
        type == "based" and is >> std::ws >> skip("on position of letter"s) >> letter) {
      op = {.type = Operation::Type::rotate_index_of, .letter = letter};
    } else if (unsigned steps{}; (type == "left" or type == "right")
                                 and is >> steps >> std::ws >> skip("step"s)
                                 and (steps == 1 or is >> skip("s"s))) {
      op
          = {.type = type == "left" ? Operation::Type::rotate_left : Operation::Type::rotate_right,
             .steps = steps};
    } else {
      throw std::runtime_error(std::format("unknown rotate type '{}'", type));
    }
  }
  return is;
}

struct ReverseOp {
  unsigned lhs{};
  unsigned rhs{};
};

std::istream& operator>>(std::istream& is, ReverseOp& op) {
  if (unsigned lhs{}, rhs{};
      is >> std::ws >> skip("positions"s) >> lhs >> std::ws >> skip("through"s) >> rhs
      and lhs < rhs) {
    op = {lhs, rhs};
  }
  return is;
}

struct MoveOp {
  unsigned src{};
  unsigned dst{};
};

std::istream& operator>>(std::istream& is, MoveOp& op) {
  if (unsigned src{}, dst{};
      is >> std::ws >> skip("position"s) >> src >> std::ws >> skip("to position"s) >> dst) {
    op = {src, dst};
  }
  return is;
}

std::istream& operator>>(std::istream& is, Operation& op) {
  if (std::string type; is >> type) {
    if (SwapOp swap_op; type == "swap" and is >> swap_op) {
      op = {
          .type = swap_op.type,
          .idx0 = swap_op.lhs,
          .idx1 = swap_op.rhs,
          .val0 = swap_op.a,
          .val1 = swap_op.b,
      };
    } else if (RotateOp rotate_op; type == "rotate" and is >> rotate_op) {
      op = {
          .type = rotate_op.type,
          .idx0 = rotate_op.steps,
          .val0 = rotate_op.letter,
      };
    } else if (ReverseOp reverse_op; type == "reverse" and is >> reverse_op) {
      op = {
          .type = Operation::Type::reverse,
          .idx0 = reverse_op.lhs,
          .idx1 = reverse_op.rhs,
      };
    } else if (MoveOp move_op; type == "move" and is >> move_op) {
      op = {
          .type = Operation::Type::move,
          .idx0 = move_op.src,
          .idx1 = move_op.dst,
      };
    } else {
      throw std::runtime_error(std::format("failed parsing unknown '{}' operation", type));
    }
  }
  return is;
}

int main() {
  const auto operations{aoc::parse_items<Operation>("/dev/stdin")};

  const auto part1{ranges::fold_left(operations, "abcdefgh"s, scramble)};
  const auto part2{ranges::fold_left(invert(operations), "fbgdceah"s, scramble)};

  std::println("{} {}", part1, part2);

  return 0;
}
