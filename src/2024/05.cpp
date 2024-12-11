#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

bool is_valid(const auto& rules, const auto& update) {
  std::unordered_set<int> seen;
  for (int page : update | views::reverse) {
    if (seen.contains(page)) {
      return false;
    }
    for (std::deque q{page}; not q.empty(); q.pop_front()) {
      int next{q.front()};
      if (auto [_, is_new]{seen.insert(next)}; is_new) {
        if (rules.contains(next)) {
          q.append_range(rules.at(next) | views::filter([&update](int r) {
                           return ranges::contains(update, r);
                         }));
        }
      }
    }
  }
  return true;
}

auto find_part1(const auto& rules, const auto& updates) {
  int n{};
  for (const auto& update : updates) {
    if (is_valid(rules, update)) {
      n += update[std::midpoint(0UZ, update.size())];
    }
  }
  return n;
}

auto find_part2(const auto& rules, const auto& updates) {
  int n{};
  for (const auto& update : updates) {
    if (not is_valid(rules, update)) {
      std::vector<int> old_update{update};
      std::vector<int> new_update;
      while (not old_update.empty()) {
        auto it{ranges::find_if(old_update, [&](int page) {
          for (auto [k, v] : rules) {
            if (ranges::contains(old_update, k) and ranges::contains(v, page)) {
              return false;
            }
          }
          return true;
        })};
        if (it == old_update.end()) {
          throw std::runtime_error("failed finding next page");
        }
        new_update.push_back(*it);
        old_update.erase(it);
      }
      n += new_update[std::midpoint(0UZ, new_update.size())];
    }
  }
  return n;
}

auto parse_input(std::string path) {
  using std::operator""s;

  auto sections{
      views::split(aoc::slurp_file(path), "\n\n"s)
      | views::transform([](auto&& s) { return ranges::to<std::string>(s); })
      | ranges::to<std::vector>()
  };

  if (sections.size() != 2) {
    throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
  }

  std::unordered_map<int, std::vector<int>> rules;
  {
    ranges::replace(sections[0], '|', ' ');
    int a{};
    int b{};
    for (std::istringstream is{sections[0]}; is >> a >> b;) {
      rules[a].push_back(b);
    }
  }

  std::vector<std::vector<int>> updates;
  {
    ranges::replace(sections[1], ',', ' ');
    for (auto&& update : views::split(sections[1], "\n"s)) {
      if (not update.empty()) {
        std::istringstream is{ranges::to<std::string>(update)};
        updates.push_back(views::istream<int>(is) | ranges::to<std::vector>());
      }
    }
  }

  return std::pair{rules, updates};
}

int main() {
  const auto [rules, updates]{parse_input("/dev/stdin")};

  const auto part1{find_part1(rules, updates)};
  const auto part2{find_part2(rules, updates)};

  std::println("{} {}", part1, part2);

  return 0;
}
