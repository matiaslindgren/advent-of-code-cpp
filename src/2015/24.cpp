import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0L, std::plus{})};
constexpr auto product{std::__bind_back(my_std::ranges::fold_left, 1L, std::multiplies{})};

std::vector<Ints> combinations_with_sum(const Ints& ints, const int k, const long target) {
  std::vector<bool> selected(ints.size(), false);
  for (std::size_t i{1}; i <= k; ++i) {
    selected[ints.size() - i] = true;
  }
  std::vector<Ints> results;
  do {
    // clang-format off
    const Ints candidate{
      views::zip(selected, ints)
      | views::filter([](const auto& p) { return p.first; })
      | views::elements<1>
      | ranges::to<Ints>()
    };
    // clang-format on
    if (sum(candidate) == target) {
      results.push_back(candidate);
    }
  } while (ranges::next_permutation(selected).found);
  return results;
}

long optimize_qe(const Ints& packages, const int group_count) {
  const long target_sum{sum(packages) / group_count};
  for (int k{1}; k < packages.size(); ++k) {
    const auto good_1st_groups{combinations_with_sum(packages, k, target_sum)};
    if (good_1st_groups.empty()) {
      continue;
    }
    // why is not required to check the other groups???
    const auto all_qe{views::transform(good_1st_groups, product)};
    return ranges::min(all_qe);
  }
  throw std::runtime_error("oh no");
}

int main() {
  const auto packages{aoc::slurp<int>("/dev/stdin")};
  const auto part1{optimize_qe(packages, 3)};
  const auto part2{optimize_qe(packages, 4)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
