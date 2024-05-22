#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};
constexpr auto product{std::__bind_back(ranges::fold_left, 1L, std::multiplies{})};

long optimize_qe(const Ints& packages, const int group_count) {
  const long target_sum{sum(packages) / group_count};

  for (int k{1}; k < packages.size(); ++k) {
    std::vector<bool> selected(packages.size(), false);
    for (std::size_t i{1}; i <= k; ++i) {
      selected.at(packages.size() - i) = true;
    }

    std::vector<Ints> good_1st_groups;
    do {
      const Ints candidate{
          views::zip(selected, packages) | views::filter([](const auto& p) { return p.first; })
          | views::elements<1> | ranges::to<Ints>()
      };
      if (sum(candidate) == target_sum) {
        good_1st_groups.push_back(candidate);
      }
    } while (ranges::next_permutation(selected).found);
    if (not good_1st_groups.empty()) {
      // apparently checking other groups is not required
      return ranges::min(views::transform(good_1st_groups, product));
    }
  }

  throw std::runtime_error("oh no");
}

int main() {
  const auto packages{aoc::parse_items<int>("/dev/stdin")};

  const auto part1{optimize_qe(packages, 3)};
  const auto part2{optimize_qe(packages, 4)};

  std::println("{} {}", part1, part2);

  return 0;
}
