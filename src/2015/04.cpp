import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto parallel_chunk_size{1uz << 12};
const auto thread_count{aoc::cpu_count()};
std::vector<std::size_t> results(thread_count);
std::vector<std::thread> threads(thread_count);

struct find_next {
  void operator()(
      const auto i_out,
      const auto begin,
      const auto count,
      const auto num_zeros,
      md5::Message msg,
      const auto input_size
  ) const {
    for (auto i{begin}; i < begin + count; ++i) {
      msg.len = input_size;
      md5::append_digits(msg, i);
      const auto checksum{md5::compute(msg)};
      if (!ranges::any_of(checksum | views::take(num_zeros), std::identity{})) {
        results[i_out] = i;
        return;
      }
    }
  }
};

std::size_t parallel_find_next(const auto begin, const auto num_zeros, const auto msg) {
  for (auto i{begin}; i < 10'000'000; i += threads.size() * parallel_chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(
          find_next{},
          t,
          i + t * parallel_chunk_size,
          parallel_chunk_size,
          num_zeros,
          msg,
          msg.len
      );
    }
    for (auto& th : threads) {
      th.join();
    }
    if (const auto r{ranges::find_if(results, std::identity{})}; r != results.end()) {
      return *r;
    }
  }
  return {};
}

int main() {
  aoc::init_io();

  md5::Message msg;
  std::cin >> msg;

  const auto part1{parallel_find_next(0uz, 5, msg)};
  ranges::fill(results, 0);
  const auto part2{parallel_find_next(part1, 6, msg)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
