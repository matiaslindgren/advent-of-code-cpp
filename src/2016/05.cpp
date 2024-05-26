#include "aoc.hpp"
#include "md5.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Result {
  static constexpr auto password_len{8UL};
  std::string pw1;
  std::string pw2;

  Result() : pw2(Result::password_len, 0) {
  }

  [[nodiscard]]
  bool is_complete() const {
    return pw1.size() == password_len and pw2.find('\0') == std::string::npos;
  }
};

struct ThreadPool {
  static constexpr auto chunk_size{1UL << 18};
  std::vector<std::thread> threads;
  std::vector<Result> results;

  explicit ThreadPool(unsigned n_threads) : threads(n_threads), results(n_threads) {
  }

  void join() {
    ranges::for_each(threads, [](auto& t) { t.join(); });
  }

  void find_passwords(std::string_view msg, const int chunk_id, const int thread_id) {
    Result res;
    const auto offset{thread_id * chunk_size};
    const auto begin{chunk_id + offset};
    for (auto i{begin}; i < begin + chunk_size and not res.is_complete(); ++i) {
      const auto sum{md5::sum32bit(std::format("{}{:d}", msg, i))};
      if ((sum & 0xfffff000) != 0U) {
        continue;
      }
      const auto pw_idx{(sum >> 8) & 0xf};
      if (res.pw1.size() < Result::password_len) {
        res.pw1 += std::format("{:x}", pw_idx);
      }
      if (pw_idx < Result::password_len and res.pw2[pw_idx] == 0) {
        const auto pw_str{std::format("{:x}", (sum >> 4) & 0xf)};
        res.pw2[pw_idx] = pw_str.front();
      }
    }
    results.at(thread_id) = res;
  }
};

Result parallel_find_passwords(std::string_view msg) {
  constexpr int max_iterations{100'000'000};
  ThreadPool pool(std::max(1U, std::thread::hardware_concurrency()));

  Result res;
  for (std::size_t i{}; i < max_iterations; i += ThreadPool::chunk_size * pool.results.size()) {
    for (auto&& [t, th] : my_std::views::enumerate(pool.threads)) {
      th = std::thread(&ThreadPool::find_passwords, &pool, msg, i, t);
    }
    pool.join();
    for (const Result& th_res : pool.results) {
      for (auto ch : th_res.pw1) {
        if (res.pw1.size() < Result::password_len) {
          res.pw1.push_back(ch);
        }
      }
      for (auto&& [pos, dst] : my_std::views::enumerate(res.pw2)) {
        if (auto src{th_res.pw2[pos]}; src != 0 and dst == 0) {
          dst = src;
        }
      }
    }
    if (res.is_complete()) {
      return res;
    }
  }

  throw std::runtime_error("search space exhausted, no answer");
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string msg;
  std::cin >> msg;

  const auto [part1, part2]{parallel_find_passwords(msg)};

  std::println("{} {}", part1, part2);
  return 0;
}
