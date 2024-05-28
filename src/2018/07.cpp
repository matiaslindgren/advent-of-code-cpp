#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Step {
  char a{};
  char b{};
};

auto run_tasks(const auto& steps, const auto n_workers) {
  using Char2Chars = std::unordered_map<char, std::unordered_set<char>>;
  Char2Chars before;
  Char2Chars after;
  for (const Step& step : steps) {
    before[step.a];
    before[step.b].insert(step.a);
    after[step.b];
    after[step.a].insert(step.b);
  }

  using IntPairs = std::vector<std::pair<int, int>>;
  IntPairs todo;
  IntPairs doing;

  const auto push_min_heap{[](auto& q, int a, int b = 0) {
    q.emplace_back(a, b);
    ranges::push_heap(q, ranges::greater{});
  }};

  const auto pop_min_heap{[](auto& q) {
    ranges::pop_heap(q, ranges::greater{});
    auto item{q.back()};
    q.pop_back();
    return item;
  }};

  for (const auto& [work, prereq] : before) {
    if (prereq.empty()) {
      push_min_heap(todo, work);
    }
  }

  std::string completed;
  int t{};

  for (; not(todo.empty() and doing.empty());
       ++t, ranges::for_each(doing, [](auto& w) { w.first -= 1; })) {
    while (not doing.empty() and doing.front().first == 0) {
      const auto done{pop_min_heap(doing).second};
      completed.push_back(done);
      for (const char next : after[done]) {
        before[next].erase(done);
        if (before[next].empty()) {
          push_min_heap(todo, next);
        }
      }
      after.erase(done);
    }
    while (not todo.empty() and doing.size() < n_workers) {
      const auto work{pop_min_heap(todo).first};
      push_min_heap(doing, 61 + (work - 'A'), work);
    }
  }

  if (not after.empty()) {
    throw std::runtime_error("graph is not a DAG");
  }
  return std::pair{completed, t - 1};
}

std::istream& operator>>(std::istream& is, Step& step) {
  if (char a{}, b{}; is >> std::ws >> skip("Step"s) >> a >> std::ws
                     >> skip("must be finished before step"s) >> b >> std::ws
                     >> skip("can begin."s)) {
    step = {a, b};
  }
  return is;
}

int main() {
  const auto steps{aoc::parse_items<Step>("/dev/stdin")};

  const auto part1{run_tasks(steps, 1).first};
  const auto part2{run_tasks(steps, 5).second};

  std::println("{} {}", part1, part2);

  return 0;
}
