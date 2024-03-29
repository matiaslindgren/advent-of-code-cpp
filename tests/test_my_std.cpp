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

void test_ranges_cartesian_product() {
  for (int n{}; n < 5; ++n) {
    std::vector<std::tuple<int, double, char>> expect;
    for (int a{}; a < n; ++a) {
      for (int b{}; b < n + 1; ++b) {
        for (int c{}; c < n + 2; ++c) {
          expect.emplace_back(a + 1, b / 10.0, 'a' + c);
        }
      }
    }

    // clang-format off
    const auto v1{
      std::views::iota(0)
      | std::views::drop(1)
      | std::views::take(n)
      | std::ranges::to<std::vector>()
    };
    const auto v2{
      std::views::iota(0)
      | std::views::transform([](auto&& b) -> double { return b / 10.0; })
      | std::views::take(n + 1)
      | std::ranges::to<std::vector>()
    };
    const auto v3{
      std::views::iota(0)
      | std::views::transform([](auto&& c) -> char { return 'a' + c; })
      | std::views::take(n + 2)
      | std::ranges::to<std::vector>()
    };
    // clang-format on

    auto it{expect.begin()};

    for (auto&& item : my_std::views::cartesian_product(v1, v2, v3)) {
      if (it == expect.end()) {
        throw std::runtime_error("cartesian product produced too many values");
      }
      if (const auto expected{*it}; expected != item) {
        throw std::runtime_error(std::format("expected {} != {} item", expected, item));
      }
      ++it;
    }

    if (it != expect.end()) {
      throw std::runtime_error("cartesian product produced too few values");
    }
  }
}

int main() {
  test_ranges_fold();
  test_ranges_stride();
  test_ranges_enumerate();
  test_ranges_cartesian_product();
  return 0;
}
