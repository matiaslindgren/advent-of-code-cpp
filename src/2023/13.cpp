import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus<int>())
};

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
    std::vector<int> row_diff;
    for (auto row1{0uz}; row1 < height; ++row1) {
      for (auto row2{0uz}; row2 < height; ++row2) {
        row_diff.push_back(ranges::count_if(views::iota(0uz, width), [&](auto x) {
          return chars[row1 * width + x] != chars[row2 * width + x];
        }));
      }
    }
    std::vector<int> col_diff;
    for (auto col1{0uz}; col1 < width; ++col1) {
      for (auto col2{0uz}; col2 < width; ++col2) {
        col_diff.push_back(ranges::count_if(views::iota(0uz, height), [&](auto y) {
          return chars[y * width + col1] != chars[y * width + col2];
        }));
      }
    }
    m = {
        .rows = {height, row_diff},
        .cols = { width, col_diff}
    };
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
  std::ios_base::sync_with_stdio(false);

  std::vector<Mirrors> mirrors;
  while (std::cin >> mirrors.emplace_back()) {
  }

  const auto part1{sum(summarize(mirrors, 0))};
  const auto part2{sum(summarize(mirrors, 1))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
