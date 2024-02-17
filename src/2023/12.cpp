import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Spring : char {
  normal = '.',
  damaged = '#',
  unknown = '?',
};

std::istream& operator>>(std::istream& is, Spring& spring) {
  if (char ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Spring::normal):
      case std::to_underlying(Spring::damaged):
      case std::to_underlying(Spring::unknown): {
        spring = {ch};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Spring status");
}

struct Springs {
  std::vector<Spring> status;
  std::vector<int> counts;
};

std::istream& operator>>(std::istream& is, Springs& springs) {
  if (std::string lhs, rhs; is >> lhs and not lhs.empty() and is >> rhs and not rhs.empty()) {
    ranges::replace(rhs, ',', ' ');
    std::stringstream lhs_s{lhs}, rhs_s{rhs};
    springs = {
        .status = views::istream<Spring>(lhs_s) | ranges::to<std::vector>(),
        .counts = views::istream<int>(rhs_s) | ranges::to<std::vector>(),
    };
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Springs");
}

static constexpr auto no_value{std::numeric_limits<std::size_t>::max()};

auto count_valid(const auto& springs, const auto& cache, const auto i, const auto c, const auto n) {
  const auto i_end{cache.extent(0) - 1};
  const auto c_end{cache.extent(1) - 1};
  const auto n_end{cache.extent(2) - 1};
  if (not(i <= i_end and c <= c_end and n <= n_end)) {
    return 0uz;
  }
  if (const auto res{cache[i, c, n]}; res != no_value) {
    return res;
  }
  auto res{0uz};
  if (i == i_end) {
    if (c == c_end and n == 0) {
      res = 1;
    }
  } else {
    const auto s{springs.status[i]};
    if (s == Spring::normal or s == Spring::unknown) {
      if (n == 0) {
        res = count_valid(springs, cache, i + 1, c, 0);
      } else if (c < c_end and springs.counts[c] == n) {
        res = count_valid(springs, cache, i + 1, c + 1, 0);
      }
    }
    if (s == Spring::damaged or s == Spring::unknown) {
      res += count_valid(springs, cache, i + 1, c, n + 1);
    }
  }
  cache[i, c, n] = res;
  return res;
}

auto count_valid(const auto& springs) {
  auto [status, counts] = springs;
  auto n_springs(status.size() + 1);
  auto n_counts(counts.size() + 1);
  auto max_count((*ranges::max_element(counts)) + 1);
  std::vector<std::size_t> cache_data(n_springs * n_counts * max_count, no_value);
  auto cache = std::mdspan(cache_data.data(), n_springs, n_counts, max_count);
  return count_valid(springs, cache, 0, 0, 0);
}

auto repeat_and_count_valid(std::ranges::range auto&& springs, const auto repeats) {
  return views::transform(springs, [=](auto s) {
    auto [status, counts] = s;
    for (auto r{1}; r < repeats; ++r) {
      s.status.push_back(Spring::unknown);
      s.status.append_range(status);
      s.counts.append_range(counts);
    }
    s.status.push_back(Spring::normal);
    return count_valid(s);
  });
};

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto springs{views::istream<Springs>(input) | ranges::to<std::vector>()};

  const auto part1{sum(repeat_and_count_valid(springs, 1))};
  const auto part2{sum(repeat_and_count_valid(springs, 5))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
