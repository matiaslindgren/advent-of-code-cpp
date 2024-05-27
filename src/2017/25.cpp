#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Test {
  int read{};
  int write{};
  int move{};
  int next{};
};

struct State {
  int id{};
  Test a;
  Test b;
};

auto find_part1(const auto begin, const auto n_steps, const auto& states) {
  std::unordered_map<int, bool> mem;
  auto state{states.at(begin)};
  for (int slot{}, step{}; step < n_steps;) {
    const auto test{mem[slot] ? state.b : state.a};
    mem[slot] = test.write;
    slot += test.move;
    step += 1;
    state = states.at(test.next);
  }
  return ranges::count_if(mem | views::values, std::identity{});
}

std::istream& operator>>(std::istream& is, Test& test) {
  if (int read{}; is >> std::ws >> skip("If the current value is "s) >> read
                  and (read == 0 or read == 1) and is >> skip(":"s)) {
    if (int write{}; is >> std::ws >> skip("- Write the value "s) >> write
                     and (write == 0 or write == 1) and is >> skip("."s)) {
      if (std::string move_str; is >> std::ws >> skip("- Move one slot to the "s) >> move_str) {
        if (const int move{
                int{move_str.starts_with("right")} - int{move_str.starts_with("left")}
            }) {
          if (char next_id{}; is >> std::ws >> skip("- Continue with state "s) >> next_id
                              and 'A' <= next_id and is >> skip("."s)) {
            test = {read, write, move, next_id - 'A'};
          }
        }
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Test");
}

std::istream& operator>>(std::istream& is, State& state) {
  if (char id{}; is >> std::ws >> skip("In state "s) >> id and 'A' <= id and is >> skip(":"s)) {
    if (Test a, b; is >> a >> b) {
      state = {id - 'A', a, b};
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing State");
}

auto parse_start_state(std::istream& is) {
  if (char ch{}; is >> skip("Begin in state "s) >> ch >> skip("."s)) {
    if (int steps{}; is >> std::ws >> skip("Perform a diagnostic checksum after "s) >> steps
                     >> skip(" steps."s)) {
      return std::pair{ch - 'A', steps};
    }
  }
  throw std::runtime_error("failed parsing start state");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto [begin, steps]{parse_start_state(input)};
  const auto states{views::istream<State>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(begin, steps, states)};
  std::print("{}\n", part1);

  return 0;
}
