import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Dep {
  char a{}, b{};
};

std::istream& operator>>(std::istream& is, Dep& dep) {
  using aoc::skip;
  using std::operator""s;
  if (char a; is >> std::ws && skip(is, "Step"s) && is >> a) {
    if (char b; is >> std::ws && skip(is, "must be finished before step"s) && is >> b >> std::ws
                && skip(is, "can begin."s)) {
      dep = {a, b};
    }
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Dep");
}

auto run_tasks(const auto& deps, const auto n_workers) {
  std::unordered_map<char, std::unordered_set<char>> before, after;
  for (const auto& dep : deps) {
    before[dep.a];
    before[dep.b].insert(dep.a);
    after[dep.b];
    after[dep.a].insert(dep.b);
  }

  std::vector<std::pair<int, int>> todo, doing;

  const auto push_min_heap{[](auto& q, int a, int b = 0) {
    q.emplace_back(a, b);
    ranges::push_heap(q, ranges::greater{});
  }};

  const auto pop_min_heap{[](auto& q) {
    ranges::pop_heap(q, ranges::greater{});
    const auto item{q.back()};
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

  for (; !(todo.empty() && doing.empty());
       ++t, ranges::for_each(doing, [](auto& w) { w.first -= 1; })) {
    while (!doing.empty() && doing.front().first == 0) {
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
    while (!todo.empty() && doing.size() < n_workers) {
      const auto work{pop_min_heap(todo).first};
      push_min_heap(doing, 61 + (work - 'A'), work);
    }
  }

  if (!after.empty()) {
    throw std::runtime_error("graph is not a DAG");
  }
  return std::pair{completed, t - 1};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto deps{views::istream<Dep>(input) | ranges::to<std::vector>()};

  const auto part1{run_tasks(deps, 1).first};
  const auto part2{run_tasks(deps, 5).second};

  std::print("{} {}\n", part1, part2);

  return 0;
}
