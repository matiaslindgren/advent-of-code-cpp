import std;
import aoc;
import my_std;

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Operation {
  enum Type {
    swap_index,
    swap_value,
    rotate_index_of,
    rotate_index_of_inverse,
    rotate_left,
    rotate_right,
    reverse,
    move,
  } type;
  unsigned idx0{}, idx1{};
  char val0{}, val1{};
};

std::istream& operator>>(std::istream& is, Operation& op) {
  using aoc::skip;
  using Type = Operation::Type;
  if (std::string type; is >> type) {
    if (type == "swap") {
      if (is >> type) {
        if (type == "position") {
          if (unsigned lhs, rhs; is >> lhs >> std::ws && skip(is, "with position"s) && is >> rhs) {
            op = {.type = Type::swap_index, .idx0 = lhs, .idx1 = rhs};
            return is;
          }
        } else if (type == "letter") {
          if (char a, b; is >> a >> std::ws && skip(is, "with letter"s) && is >> b) {
            op = {.type = Type::swap_value, .val0 = a, .val1 = b};
            return is;
          }
        }
      }
    } else if (type == "rotate") {
      if (is >> type) {
        if (type == "based") {
          if (char letter; is >> std::ws && skip(is, "on position of letter"s) && is >> letter) {
            op = {.type = Type::rotate_index_of, .val0 = letter};
            return is;
          }
        } else if (type == "left" || type == "right") {
          if (unsigned steps;
              is >> steps >> std::ws && skip(is, "step"s) && (steps == 1 || skip(is, "s"s))) {
            op = {.type = type == "left" ? Type::rotate_left : Type::rotate_right, .idx0 = steps};
            return is;
          }
        }
      }
    } else if (type == "reverse") {
      if (unsigned lhs, rhs; is >> std::ws && skip(is, "positions"s) && is >> lhs >> std::ws
                             && skip(is, "through"s) && is >> rhs && lhs < rhs) {
        op = {.type = Type::reverse, .idx0 = lhs, .idx1 = rhs};
        return is;
      }
    } else if (type == "move") {
      if (unsigned src, dst; is >> std::ws && skip(is, "position"s) && is >> src >> std::ws
                             && skip(is, "to position"s) && is >> dst) {
        op = {.type = Type::move, .idx0 = src, .idx1 = dst};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Operation");
}

void rotate_left(std::string& s, const auto n) {
  ranges::rotate(s, s.begin() + (n % s.size()));
}

void rotate_right(std::string& s, const auto n) {
  ranges::rotate(s, s.end() - (n % s.size()));
}

std::string scramble(std::string input, ranges::forward_range auto&& operations) {
  return my_std::ranges::fold_left(operations, input, [](std::string s, auto op) {
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
        op.idx0 = 1 + (i / 2) + 4 * (i % 2 == 0);
        [[fallthrough]];
      }
      case Type::rotate_left:
        rotate_left(s, op.idx0);
        break;
      case Type::rotate_index_of: {
        auto i{s.find(op.val0)};
        op.idx0 = i + 1 + (i > 3);
        [[fallthrough]];
      }
      case Type::rotate_right:
        rotate_right(s, op.idx0);
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
  });
}

auto invert(ranges::forward_range auto&& operations) {
  return views::transform(operations | views::reverse, [](const auto& op) {
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

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto operations{views::istream<Operation>(input) | ranges::to<std::vector>()};

  const auto part1{scramble("abcdefgh"s, operations)};
  const auto part2{scramble("fbgdceah"s, invert(operations))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
