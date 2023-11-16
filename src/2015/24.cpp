import std;

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

long sum(const Ints& ints) {
  return std::accumulate(ints.begin(), ints.end(), 0L);
};
long qe(const Ints& g) {
  return std::accumulate(g.begin(), g.end(), 1L, std::multiplies<long>());
};

std::vector<Ints> combinations(const Ints& ints, const int k, const long target_sum) {
  std::vector<bool> selected(ints.size(), false);
  for (std::size_t i{1}; i <= k; ++i) {
    selected[ints.size() - i] = true;
  }
  std::vector<Ints> results;
  do {
    // clang-format off
    const Ints comb{
      views::zip(ints, selected)
      | views::filter([](auto&& pair) { return pair.second; })
      | views::transform([](auto&& pair) { return pair.first; })
      | ranges::to<Ints>()
    };
    // clang-format on
    if (sum(comb) == target_sum) {
      results.push_back(comb);
    }
  } while (ranges::next_permutation(selected).found);
  return results;
}

long optimize_qe(const Ints& packages, const int group_count) {
  const long target_sum{sum(packages) / group_count};
  for (int k{1}; k < packages.size(); ++k) {
    const auto good_first_groups = combinations(packages, k, target_sum);
    if (!good_first_groups.empty()) {
      const auto best_group1{ranges::min_element(good_first_groups, {}, qe)};
      return qe(*best_group1);
    }
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto packages = views::istream<int>(std::cin) | ranges::to<Ints>();
  const auto part1{optimize_qe(packages, 3)};
  const auto part2{optimize_qe(packages, 4)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
