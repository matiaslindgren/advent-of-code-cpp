import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

struct Diffs {
  std::size_t width;
  std::vector<int> diff;

  std::size_t find_reflection(const int fix_count) const {
    /*
     *            0 1 2 3 4 5 6
     *  #.##..##. 0
     *  ..#.##.#. 5 0
     *  ##......# 5 7 0
     *  ##......# 5 7 0 0
     *  ..#.##.#. 5 0 7 7 0
     *  ..##..##. 1 4 7 7 4 0
     *  #.#.##.#. 4 1 6 6 1 5 0
     *
     */
    for (auto i{1uz}; i < width; ++i) {
      const auto diffs{views::iota(0uz, std::min(i, width - i)) | views::transform([&](auto size) {
                         const auto lhs{i - 1 - size};
                         const auto rhs{i + size};
                         const auto d1{diff[lhs * width + lhs]};
                         const auto d2{diff[rhs * width + lhs]};
                         return d1 + d2;
                       })};
      if (sum(diffs) == fix_count) {
        return i;
      }
    }
    return {};
  }
};

struct Mirrors {
  Diffs rows;
  Diffs cols;
};

auto pairwise_diff(
    const auto& v,
    const auto n,
    const auto m,
    const auto stride1,
    const auto stride2
) {
  return (
      views::iota(0uz, n * n) | views::transform([&](auto i) -> int {
        const auto i1{i / n};
        const auto i2{i % n};
        return ranges::count_if(views::iota(0uz, m), [&](auto j) {
          const auto x1{v[i1 * stride1 + j * stride2]};
          const auto x2{v[i2 * stride1 + j * stride2]};
          return x1 != x2;
        });
      })
      | ranges::to<std::vector>()
  );
}

std::istream& operator>>(std::istream& is, Mirrors& m) {
  std::vector<char> chars;
  auto width{0uz};
  for (std::string line; std::getline(is, line) && !line.empty();) {
    if (chars.empty() || line.size() == width) {
      width = line.size();
      chars.append_range(line);
    }
  }
  if (!chars.empty()) {
    const auto height{chars.size() / width};
    const auto& row_diff{pairwise_diff(chars, height, width, width, 1)};
    const auto& col_diff{pairwise_diff(chars, width, height, 1, width)};
    m = {.rows = {height, row_diff}, .cols = {width, col_diff}};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Mirrors");
}

auto summarize(const auto& mirrors, const auto fix_count) {
  return views::transform(mirrors, [=](const auto& m) {
    const auto cols{m.cols.find_reflection(fix_count)};
    const auto rows{m.rows.find_reflection(fix_count)};
    return cols + 100 * rows;
  });
}

int main() {
  aoc::init_io();

  // TODO views::istream
  std::vector<Mirrors> mirrors;
  while (std::cin >> mirrors.emplace_back()) {
  }

  const auto part1{sum(summarize(mirrors, 0))};
  const auto part2{sum(summarize(mirrors, 1))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
