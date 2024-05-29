#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

enum struct BinOp : unsigned char {
  multiply,
  add,
  square,
};

struct Operation {
  BinOp op{};
  int operand{};

  [[nodiscard]]
  long compute(long old) const {
    switch (op) {
      case BinOp::multiply:
        return old * operand;
      case BinOp::add:
        return old + operand;
      case BinOp::square:
        return old * old;
    }
  }
};

using Items = std::vector<int>;

struct Monkey {
  int id{};
  Operation op;
  int test_divisor{};
  int true_dst{};
  int false_dst{};
  Items items;
  std::size_t n_inspected{};

  [[nodiscard]]
  auto get_dst(auto item) const {
    return item % test_divisor == 0 ? true_dst : false_dst;
  }
};

constexpr auto product{std::__bind_back(ranges::fold_left, 1L, std::multiplies{})};

auto run_monkey_business(auto monkeys, int worry_divisor, int n_rounds) {
  const auto test_divisors_lcm{product(views::transform(monkeys, &Monkey::test_divisor))};
  while (n_rounds-- > 0) {
    for (Monkey& monkey : monkeys) {
      const auto new_items{
          monkey.items | views::transform([&](long item) {
            return (monkey.op.compute(item) / worry_divisor) % test_divisors_lcm;
          })
          | ranges::to<std::vector>()
      };
      monkey.n_inspected += monkey.items.size();
      monkey.items.clear();
      ranges::for_each(new_items, [&](long item) {
        monkeys.at(monkey.get_dst(item)).items.push_back(item);
      });
    }
  }
  auto inspected{monkeys | views::transform(&Monkey::n_inspected) | ranges::to<std::set>()};
  return product(inspected | views::drop(inspected.size() - 2));
}

std::istream& operator>>(std::istream& is, Items& items) {
  if (std::string line;
      is >> std::ws >> skip("Starting items:"s) and std::getline(is, line) and not line.empty()) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    items = views::istream<int>(ls) | ranges::to<std::vector>();
    if (items.empty() or not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing items from line '{}'", line));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Operation& operation) {
  if (std::string opname, rhs; is >> std::ws >> skip("Operation: new = old"s) >> opname >> rhs) {
    BinOp op{};
    int operand{};
    if (opname == "*"s and rhs == "old"s) {
      op = BinOp::square;
    } else if ((operand = std::atoi(rhs.c_str())) != 0 and opname == "+") {
      op = BinOp::add;
    } else if (operand != 0 and opname == "*") {
      op = BinOp::multiply;
    } else {
      throw std::runtime_error(
          std::format("invalid binary operation '{}' with rhs '{}'", opname, rhs)
      );
    }
    operation = {op, operand};
  }
  return is;
}

std::istream& operator>>(std::istream& is, Monkey& monkey) {
  bool ok{false};
  if (auto [id, items, op]{std::tuple{int{}, Items{}, Operation{}}};
      is >> std::ws >> skip("Monkey"s) >> id >> skip(":"s) and id >= 0 and is >> items >> op) {
    if (int test_divisor{};
        is >> std::ws >> skip("Test: divisible by"s) >> test_divisor and test_divisor > 0) {
      if (int true_dst{};
          is >> std::ws >> skip("If true: throw to monkey"s) >> true_dst and true_dst >= 0) {
        if (int false_dst{};
            is >> std::ws >> skip("If false: throw to monkey"s) >> false_dst and false_dst >= 0) {
          monkey = Monkey{
              id,
              op,
              test_divisor,
              true_dst,
              false_dst,
              items,
          };
          ok = true;
        }
      }
    }
  }
  if ((not ok or is.fail()) and not is.eof()) {
    throw std::runtime_error("failed parsing Monkey");
  }
  return is;
}

int main() {
  const auto monkeys{aoc::parse_items<Monkey>("/dev/stdin")};

  const auto part1{run_monkey_business(monkeys, 3, 20)};
  const auto part2{run_monkey_business(monkeys, 1, 10'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
