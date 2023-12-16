import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

constexpr decltype(auto) make_fold(auto&& init, auto&& f) {
  return std::bind(my_std::ranges::fold_left, std::placeholders::_1, init, f);
}

constexpr auto sum{make_fold(0L, std::plus{})};
constexpr auto product{make_fold(1L, std::multiplies{})};

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
      | views::transform([](const auto& p) { return p.second; })
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
    return *ranges::min_element(all_qe);
  }
  throw std::runtime_error("oh no");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto packages{views::istream<int>(std::cin) | ranges::to<Ints>()};
  const auto part1{optimize_qe(packages, 3)};
  const auto part2{optimize_qe(packages, 4)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
