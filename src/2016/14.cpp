#include "aoc.hpp"
#include "md5.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct ThreadPool {
  static constexpr auto chunk_size{256U};
  std::vector<std::thread> threads;
  std::vector<std::string> results;

  explicit ThreadPool(unsigned n_threads) : threads(n_threads), results(chunk_size * n_threads) {
  }

  void join() {
    ranges::for_each(threads, [](auto& t) { t.join(); });
  }

  void md5stretch(std::string_view salt, int index, int thread_id, int stretch_count) {
    const auto offset{thread_id * chunk_size};
    for (unsigned i{}; i < chunk_size; ++i) {
      auto checksum{std::format("{:s}{:d}", salt, index + offset + i)};
      for (int r{}; r <= stretch_count; r += 1) {
        checksum = md5::hexdigest(md5::sum(checksum));
      }
      results.at(offset + i) = checksum;
    }
  }

  void submit(std::string_view salt, int index, int stretch_count) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(&ThreadPool::md5stretch, this, salt, index, t, stretch_count);
    }
  }
};

// TODO (llvm19?) ranges::adjacent
inline auto window3(std::string_view s) {
  return views::zip(s, views::drop(s, 1), views::drop(s, 2));
}
inline auto window5(std::string_view s) {
  return views::zip(s, views::drop(s, 1), views::drop(s, 2), views::drop(s, 3), views::drop(s, 4));
}

inline std::optional<char> find_triple(std::string_view s) {
  for (auto&& [a, b, c] : window3(s)) {
    if (a == b and b == c) {
      return a;
    }
  }
  return std::nullopt;
}

inline bool has_quint(std::string_view s, char first) {
  for (auto&& [a, b, c, d, e] : window5(s)) {
    if (a == first and a == b and b == c and c == d and d == e) {
      return true;
    }
  }
  return false;
}

auto stretch_search(std::string_view salt, const int stretch_count = 0) {
  // adapted from
  // https://github.com/benediktwerner/AdventOfCode/blob/1d2b3687d0969fcc49d9aa45354e7f4179f57712/2016/day14/fast/src/main.rs
  // accessed 2024-05-26
  int index{};

  ThreadPool pool(std::max(1U, std::thread::hardware_concurrency()));
  std::vector<std::string> checksums;

  for (int key{}; key < 64; key += 1) {
    for (bool found{false}; not found; index += 1) {
      while (index + 1000 >= checksums.size()) {
        pool.submit(salt, checksums.size(), stretch_count);
        pool.join();
        checksums.append_range(pool.results);
      }
      const auto& checksum{checksums.at(index)};
      if (auto triple{find_triple(checksum)}) {
        found = ranges::any_of(views::iota(0, 1000), [&, target = triple.value()](int i) {
          return has_quint(checksums.at(index + 1 + i), target);
        });
      }
    }
  }

  return index - 1;
}

int main() {
  std::ios::sync_with_stdio(false);
  if (std::string salt; std::cin >> salt) {
    const auto part1{stretch_search(salt)};
    const auto part2{stretch_search(salt, 2016)};
    std::println("{} {}", part1, part2);
    return 0;
  }
  throw std::runtime_error("failed parsing input");
}
