import std;

namespace ranges = std::ranges;
namespace views = std::views;
using std::vector;

void _find_all_combinations(
    vector<vector<int>>& results,
    int target,
    vector<int> used,
    vector<int> available,
    const std::size_t pos
) {
  if (target == 0) {
    results.push_back(used);
    return;
  }
  if (target < 0 || pos == available.size()) {
    return;
  }
  _find_all_combinations(results, target, used, available, pos + 1);
  const auto it{ranges::next(available.begin(), pos)};
  target -= *it;
  used.push_back(*it);
  available.erase(it);
  _find_all_combinations(results, target, used, available, pos);
}

vector<vector<int>>
find_all_combinations(const int init_target, const vector<int>& all_containers) {
  // TODO(llvm18) deducing this, recursive lambda
  vector<vector<int>> results;
  _find_all_combinations(results, init_target, {}, all_containers, 0);
  return results;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto containers = views::istream<int>(std::cin) | ranges::to<vector<int>>();
  const auto combinations = find_all_combinations(150, containers);

  const auto part1{combinations.size()};

  const auto get_container_count{[](auto&& containers) { return containers.size(); }};
  const auto minimum_combination{
      ranges::min_element(combinations, ranges::less{}, get_container_count)};
  const auto part2{ranges::count(combinations, minimum_combination->size(), get_container_count)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
