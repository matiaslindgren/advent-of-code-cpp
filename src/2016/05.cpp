#include "aoc.hpp"
#include "md5.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Result {
  static constexpr auto password_len{8uz};
  std::string pw1;
  std::string pw2;

  explicit Result() : pw1{}, pw2(password_len, 0) {
  }

  constexpr bool is_complete() const {
    return pw1.size() == password_len and pw2.find('\0') == std::string::npos;
  }
};

constexpr auto chunk_size{1uz << 18};
const auto n_threads{aoc::cpu_count()};
std::vector<Result> results(n_threads);
std::vector<std::thread> threads(n_threads);

struct find_passwords {
  void operator()(const auto i_out, std::string_view msg, const auto begin, const auto count)
      const {
    Result res;
    for (auto i{begin}; i < begin + count and not res.is_complete(); ++i) {
      const auto sum{md5::sum32bit(std::format("{}{:d}", msg, i))};
      if (sum & 0xfffff000) {
        continue;
      }
      const auto pw_idx{(sum >> 8) & 0xf};
      if (res.pw1.size() < res.password_len) {
        res.pw1 += std::format("{:x}", pw_idx);
      }
      if (pw_idx < res.password_len and not res.pw2[pw_idx]) {
        const auto pw_str{std::format("{:x}", (sum >> 4) & 0xf)};
        res.pw2[pw_idx] = pw_str.front();
      }
    }
    results[i_out] = res;
  }
};

Result parallel_find_passwords(std::string_view msg) {
  Result res;
  for (auto i{0uz}; i < 100'000'000 and not res.is_complete(); i += threads.size() * chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(find_passwords{}, t, msg, i + t * chunk_size, chunk_size);
    }
    for (auto& th : threads) {
      th.join();
    }
    for (const auto& th_res : results) {
      for (auto ch : th_res.pw1) {
        if (res.pw1.size() < res.password_len) {
          res.pw1.push_back(ch);
        }
      }
      for (auto&& [pos, dst] : my_std::views::enumerate(res.pw2)) {
        auto src{th_res.pw2[pos]};
        if (src and not dst) {
          dst = src;
        }
      }
    }
  }
  return res;
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string msg;
  std::cin >> msg;

  const auto [part1, part2] = parallel_find_passwords(msg);

  std::print("{} {}\n", part1, part2);
  return 0;
}
