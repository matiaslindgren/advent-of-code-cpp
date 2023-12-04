import std;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto parallel_chunk_size{1uz << 12};
constexpr auto thread_count{8uz};
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
      const auto msg_len{md5::append_digits(msg, input_size, i)};
      const auto checksum{md5::compute(msg, msg_len)};
      if (!ranges::any_of(checksum | views::take(num_zeros), std::identity{})) {
        results[i_out] = i;
        return;
      }
    }
  }
};

std::size_t parallel_find_next(const auto begin, auto&&... args) {
  for (auto i{begin}; i < 10'000'000; i += threads.size() * parallel_chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(find_next{}, t, i + t * parallel_chunk_size, parallel_chunk_size, args...);
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
  std::ios_base::sync_with_stdio(false);

  md5::Message msg = {0};
  std::size_t input_size{};
  {
    std::string input;
    std::cin >> input;
    input_size = input.size();
    ranges::move(input, msg.begin());
  }

  const auto part1{parallel_find_next(0uz, 5, msg, input_size)};
  ranges::fill(results, 0);
  const auto part2{parallel_find_next(part1, 6, msg, input_size)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
