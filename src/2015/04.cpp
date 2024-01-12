import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto chunk_size{1uz << 12};
const auto n_threads{aoc::cpu_count()};
std::vector<std::size_t> results(n_threads);
std::vector<std::thread> threads(n_threads);

struct find_next {
  void operator()(
      const auto i_out,
      const auto begin,
      const auto count,
      const auto num_zeros,
      md5::Message msg
  ) const {
    const auto msg_len{msg.size()};
    for (auto i{begin}; i < begin + count; ++i) {
      md5::append_digits(msg, i);
      const auto checksum{md5::compute(msg)};
      msg.erase(msg.begin() + msg_len, msg.end());
      if (!ranges::any_of(checksum | views::take(num_zeros), std::identity{})) {
        results[i_out] = i;
        return;
      }
    }
  }
};

std::size_t parallel_find_next(const auto begin, const auto num_zeros, const auto msg) {
  for (auto i{begin}; i < 10'000'000; i += threads.size() * chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(find_next{}, t, i + t * chunk_size, chunk_size, num_zeros, msg);
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
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const md5::Message msg{md5::parse_line(input)};

  const auto part1{parallel_find_next(0uz, 5, msg)};
  ranges::fill(results, 0);
  const auto part2{parallel_find_next(part1, 6, msg)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
