#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

std::vector<std::vector<int>>
find_all_combinations(const int init_target, const std::vector<int>& all_containers) {
  std::vector<std::vector<int>> results;
  [&](this auto&& self,
      int target,
      std::vector<int> available,
      std::vector<int> used = {},
      std::size_t pos = {}) {
    if (target == 0) {
      results.push_back(used);
      return;
    }
    if (target < 0 or pos == available.size()) {
      return;
    }
    self(target, available, used, pos + 1);
    const auto it{ranges::next(available.begin(), pos)};
    target -= *it;
    used.push_back(*it);
    available.erase(it);
    self(target, available, used, pos);
  }(init_target, all_containers);
  return results;
}

int main() {
  const auto containers{aoc::parse_items<int>("/dev/stdin")};
  const auto combinations{find_all_combinations(150, containers)};

  const auto part1{combinations.size()};

  const auto get_container_count{[](const auto& c) { return c.size(); }};
  const auto minimum_combination{
      ranges::min_element(combinations, ranges::less{}, get_container_count)
  };
  const auto part2{ranges::count(combinations, minimum_combination->size(), get_container_count)};

  std::println("{} {}", part1, part2);

  return 0;
}
