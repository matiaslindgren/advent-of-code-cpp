#include "aoc.hpp"
#include "md5.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

int count_zeros(md5::Chunk sum) {
  int n{};
  for (md5::Chunk mask{0xf0000000}; mask != 0U and (sum & mask) == 0U; mask >>= 4) {
    ++n;
  }
  return n;
}

struct ThreadPool {
  static constexpr auto chunk_size{1UZ << 12};
  std::vector<std::thread> threads;
  std::vector<std::pair<int, md5::Chunk>> results;

  explicit ThreadPool(unsigned n_threads) : threads(n_threads), results(n_threads * chunk_size) {
  }

  void join() {
    ranges::for_each(threads, [](auto& t) { t.join(); });
  }

  void md5sum_32bit(std::string_view msg, const int chunk_id, const int thread_id) {
    const auto offset{thread_id * chunk_size};
    const auto begin{chunk_id + offset};
    for (int i{}; i < chunk_size; ++i) {
      results[offset + i] = {begin + i, md5::sum32bit(std::format("{}{:d}", msg, begin + i))};
    }
  }
};

auto search(std::string_view msg) {
  constexpr int max_iterations{10'000'000};
  constexpr int part1_limit{5};
  constexpr int part2_limit{6};

  ThreadPool pool(std::max(1U, std::thread::hardware_concurrency()));

  int part1{std::numeric_limits<int>::max()};
  for (std::size_t i{}; i < max_iterations; i += pool.results.size()) {
    for (auto&& [t, th] : my_std::views::enumerate(pool.threads)) {
      th = std::thread(&ThreadPool::md5sum_32bit, &pool, msg, i, t);
    }
    pool.join();
    for (const auto& [j, sum] : pool.results) {
      const auto n_zero{count_zeros(sum)};
      if (n_zero == part1_limit) {
        part1 = std::min(part1, j);
      } else if (n_zero == part2_limit) {
        return std::pair{part1, j};
      }
    }
  }

  throw std::runtime_error("search space exhausted, no answer");
}

int main() {
  std::ios::sync_with_stdio(false);
  std::string msg;
  std::cin >> msg;
  const auto [part1, part2]{search(msg)};
  std::println("{} {}", part1, part2);
  return 0;
}
