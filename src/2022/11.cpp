#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

enum struct BinOp {
  multiply,
  add,
  square,
};

struct Monkey {
  int id{};
  BinOp op;
  int operand{};
  int test_divisor{};
  int true_dst{};
  int false_dst{};
  std::vector<int> items;
  int n_inspected{};

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

  auto get_dst(int item) const {
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
            return (monkey.compute(item) / worry_divisor) % test_divisors_lcm;
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

std::istream& operator>>(std::istream& is, Monkey& monkey) {
  bool ok{false};
  if (int id; is >> std::ws >> skip("Monkey"s) >> id >> skip(":"s) and id >= 0) {
    if (is >> std::ws >> skip("Starting items:"s)) {
      if (std::string line; std::getline(is, line) and not line.empty()) {
        ranges::replace(line, ',', ' ');
        std::istringstream ls{line};
        if (auto items{views::istream<int>(ls) | ranges::to<std::vector>()};
            not items.empty() and ls.eof()) {
          if (std::string opname, rhs;
              is >> std::ws >> skip("Operation: new = old"s) >> opname >> rhs) {
            BinOp op;
            int operand{};
            if (opname == "*"s and rhs == "old"s) {
              op = BinOp::square;
            } else if ((operand = std::atoi(rhs.c_str())) and opname == "+") {
              op = BinOp::add;
            } else if (operand and opname == "*") {
              op = BinOp::multiply;
            } else {
              throw std::runtime_error("invalid binary operation");
            }
            if (int test_divisor;
                is >> std::ws >> skip("Test: divisible by"s) >> test_divisor and test_divisor > 0) {
              if (int true_dst; is >> std::ws >> skip("If true: throw to monkey"s) >> true_dst
                                and true_dst >= 0) {
                if (int false_dst; is >> std::ws >> skip("If false: throw to monkey"s) >> false_dst
                                   and false_dst >= 0) {
                  monkey = Monkey{
                      id,
                      op,
                      operand,
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
  const auto monkeys{aoc::slurp<Monkey>("/dev/stdin")};

  const auto part1{run_monkey_business(monkeys, 3, 20)};
  const auto part2{run_monkey_business(monkeys, 1, 10'000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
