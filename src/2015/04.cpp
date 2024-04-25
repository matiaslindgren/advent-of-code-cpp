#include "aoc.hpp"
#include "md5.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto chunk_size{1uz << 12};
std::vector<std::thread> threads(aoc::cpu_count());
std::vector<std::pair<int, md5::Chunk>> results(threads.size() * chunk_size);

struct md5sum_32bit {
  void operator()(std::string_view msg, const auto begin, const auto res_begin, const auto count)
      const {
    for (int i{0}; i < count; ++i) {
      results[res_begin + i] = {begin + i, md5::sum32bit(std::format("{}{:d}", msg, begin + i))};
    }
  }
};

int count_zeros(md5::Chunk sum) {
  int n{};
  for (md5::Chunk mask{0xf0000000}; mask and not(sum & mask); mask >>= 4) {
    ++n;
  }
  return n;
}

std::pair<int, int> search(std::string_view msg) {
  auto part1{std::numeric_limits<int>::max()};
  for (int i{}; i < 10'000'000; i += results.size()) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      const auto offset{t * chunk_size};
      th = std::thread(md5sum_32bit{}, msg, i + offset, offset, chunk_size);
    }
    ranges::for_each(threads, [](auto& th) { th.join(); });
    for (const auto& [j, sum] : results) {
      const auto n_zero{count_zeros(sum)};
      if (n_zero == 5) {
        part1 = std::min(part1, j);
      } else if (n_zero == 6) {
        return {part1, j};
      }
    }
  }
  throw std::runtime_error("search space exhausted, no answer");
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string msg;
  std::cin >> msg;

  const auto [part1, part2] = search(msg);

  std::print("{} {}\n", part1, part2);

  return 0;
}
