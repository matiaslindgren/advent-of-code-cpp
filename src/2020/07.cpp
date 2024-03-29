import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Bag {
  std::string id;
  int count{};
};

struct Rule {
  Bag bag;
  std::vector<Bag> inside;
};

std::istream& operator>>(std::istream& is, Bag& bag) {
  if (std::string a, b, tmp; is >> a >> b >> tmp) {
    if (not a.empty() and not b.empty() and tmp.starts_with("bag")) {
      bag = {.id = a + " " + b};
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::stringstream ls{line};
    if (Bag bag; ls >> bag >> std::ws >> skip("contain"s)) {
      std::vector<Bag> inside;
      while (ls) {
        if (int count; ls >> count) {
          if (Bag bag; ls >> bag) {
            inside.emplace_back(bag.id, count);
          }
        }
      }
      if (ls.eof() or line.contains("no other bags")) {
        rule = {bag, inside};
      } else {
        throw std::runtime_error(std::format("failed parsing line {}", line));
      }
    }
  }
  return is;
}

using Edges = std::unordered_map<std::string, std::unordered_map<std::string, int>>;

auto find_part1(const auto& rules) {
  Edges parents;
  for (const Rule& r : rules) {
    for (const Bag& b : r.inside) {
      parents[b.id][r.bag.id] = 0;
    }
  }
  std::unordered_set<std::string> visited;
  for (std::deque q{"shiny gold"s}; not q.empty(); q.pop_front()) {
    auto bag{q.front()};
    if (auto [_, unvisited]{visited.insert(bag)}; unvisited and parents.contains(bag)) {
      q.append_range(parents.at(bag) | views::keys);
    }
  }
  return visited.size() - 1;
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto count_bags(std::string bag, const auto& children) {
  if (not children.contains(bag)) {
    return 0;
  }
  return sum(views::transform(children.at(bag), [&children](auto&& node) {
    auto [sub_bag, count]{node};
    return count * (1 + count_bags(sub_bag, children));
  }));
}

auto find_part2(const auto& rules) {
  Edges children;
  for (const Rule& r : rules) {
    for (const Bag& b : r.inside) {
      children[r.bag.id][b.id] = b.count;
    }
  }
  return count_bags("shiny gold"s, children);
}

int main() {
  const auto rules{aoc::slurp<Rule>("/dev/stdin")};

  const auto part1{find_part1(rules)};
  const auto part2{find_part2(rules)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
