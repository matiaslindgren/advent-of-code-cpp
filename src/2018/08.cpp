import std;
import aoc;
import my_std;

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

std::pair<int, int> find_tree_value(const auto& tree, const bool part1, const int i = 0) {
  const auto n_children{tree[i]};
  const auto n_metadata{tree[i + 1]};

  int size{2};
  std::vector<int> sub_values;
  for (int c{}; c < n_children; ++c) {
    const auto [c_size, c_total]{find_tree_value(tree, part1, i + size)};
    size += c_size;
    sub_values.push_back(c_total);
  }

  int total{};
  if (part1) {
    total += sum(sub_values);
  }
  for (int j{}; j < n_metadata; ++j) {
    const auto m{tree[j + i + size]};
    if (part1 or sub_values.empty()) {
      total += m;
    } else if (0 < m and m <= n_children) {
      total += sub_values[m - 1];
    }
  }

  return {size + n_metadata, total};
}

int main() {
  const auto tree{aoc::slurp<int>("/dev/stdin")};

  const auto part1{find_tree_value(tree, true).second};
  const auto part2{find_tree_value(tree, false).second};

  std::print("{} {}\n", part1, part2);

  return 0;
}
