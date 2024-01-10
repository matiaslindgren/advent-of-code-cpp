import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

struct Repeat {
  std::size_t i;
  unsigned char len;
  unsigned char digit;
  constexpr auto operator<=>(const Repeat&) const = default;
};

template <>
struct std::hash<Repeat> {
  std::size_t operator()(const Repeat& r) const noexcept {
    return std::hash<std::size_t>{}(r.i * 16 * 16 + r.len * 16 + r.digit);
  }
};

constexpr auto parallel_chunk_size{256u};
const auto thread_count{aoc::cpu_count()};
std::vector<std::set<Repeat>> results(thread_count);
std::vector<std::thread> threads(thread_count);

// TODO ranges::adjacent
// TODO template index sequence
constexpr decltype(auto) window3(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2));
}
constexpr decltype(auto) window5(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3), views::drop(r, 4));
}

struct stretch_search {
  void operator()(
      const auto i_out,
      md5::Message msg,
      const auto input_size,
      const auto begin,
      const auto count,
      const auto stretch_count
  ) const {
    for (auto i{begin}; i < begin + count; ++i) {
      msg.len = input_size;
      md5::append_digits(msg, i);
      const auto checksum{md5::compute(msg, 1 + stretch_count)};
      for (const auto [d0, d1, d2] : window3(checksum)) {
        if (d0 == d1 && d1 == d2) {
          results[i_out].insert({i, 3u, d0});
          break;
        }
      }
      for (const auto [d0, d1, d2, d3, d4] : window5(checksum)) {
        if (d0 == d1 && d1 == d2 && d2 == d3 && d3 == d4) {
          results[i_out].insert({i, 5u, d0});
        }
      }
    }
  }
};

std::size_t parallel_stretch_search(md5::Message msg, const int stretch_count = 0) {
  using Keys = std::set<std::size_t>;
  using DigitRepeats = std::unordered_map<unsigned char, Keys>;
  DigitRepeats r2s, r5s;
  Keys keys;
  for (auto i{0uz}; keys.size() < 70 && i < 1'000'000; i += threads.size() * parallel_chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      results[t].clear();
      th = std::thread(
          stretch_search{},
          t,
          msg,
          msg.len,
          i + t * parallel_chunk_size,
          parallel_chunk_size,
          stretch_count
      );
    }
    for (auto& th : threads) {
      th.join();
    }
    for (const auto& repeats : results) {
      for (const auto& r : repeats) {
        (r.len == 5 ? r5s : r2s)[r.digit].insert(r.i);
      }
    }
    for (auto digit{0u}; digit < 16; ++digit) {
      for (const auto& r5_i : r5s[digit]) {
        for (const auto& r2_i : r2s[digit]) {
          if (const auto dist{r5_i - r2_i}; 0 < dist && dist <= 1000) {
            keys.insert(r2_i);
          }
        }
      }
    }
  }
  if (const auto key{ranges::begin(views::drop(keys, 63))}; key != keys.end()) {
    return *key;
  }
  return std::numeric_limits<std::size_t>::max();
}

int main() {
  aoc::init_io();

  md5::Message msg;
  std::cin >> msg;

  const auto part1{parallel_stretch_search(msg)};
  const auto part2{parallel_stretch_search(msg, 2016)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
