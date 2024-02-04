import std;
import my_std;

void test_ranges_fold() {
  using std::operator""s;

  const auto s{"abcdef"s};
  {
    const auto expect{"."s + s};
    const auto result{my_std::ranges::fold_left(s, "."s, std::plus{})};
    if (result != expect) {
      throw std::runtime_error(std::format("fold left concat: {} != {}", expect, result));
    }
  }
  {
    const auto expect{s + "."s};
    const auto result{my_std::ranges::fold_right(s, "."s, std::plus{})};
    if (result != expect) {
      throw std::runtime_error(std::format("fold right concat: {} != {}", expect, result));
    }
  }
}

void test_ranges_stride() {
  constexpr auto n{41};

  for (int s{1}; s < n; ++s) {
    std::vector<int> expect;
    for (int i{}; i < n; i += s) {
      expect.push_back(i);
    }

    auto it{expect.begin()};

    for (const auto i : std::views::iota(0, n) | my_std::views::stride(s)) {
      if (it == expect.end()) {
        throw std::runtime_error("stride produced too many values");
      }
      if (const auto expect{*it}; expect != i) {
        throw std::runtime_error(std::format("stride mismatch: {} != {}", expect, i));
      }
      ++it;
    }

    if (it != expect.end()) {
      throw std::runtime_error("stride produced too few values");
    }
  }
}

void test_ranges_enumerate() {
  const auto v{std::views::iota(0, 10) | std::ranges::to<std::vector>()};

  auto expect{0uz};
  for (auto&& [i, x] : my_std::views::enumerate(v)) {
    if (expect == v.size()) {
      throw std::runtime_error("enumerate produced too many values");
    }
    if (expect != i) {
      throw std::runtime_error(std::format("enumerate index mismatch: {} != {}", expect, i));
    }
    if (const auto y{v[i]}; y != x) {
      throw std::runtime_error(std::format("enumerate value mismatch: {} != {}", y, x));
    }
    ++expect;
  }
  if (expect != v.size()) {
    throw std::runtime_error("enumerate produced too few values");
  }
}

int main() {
  test_ranges_fold();
  test_ranges_stride();
  test_ranges_enumerate();
  return 0;
}
