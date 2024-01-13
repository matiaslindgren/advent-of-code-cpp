import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto chunk_size{512u};
std::vector<std::thread> threads(aoc::cpu_count());
std::vector<std::string> checksums(threads.size() * chunk_size);

struct md5sum_repeated {
  void operator()(std::string_view msg, const auto i_begin, const auto n_repeat, const auto stride)
      const {
    for (auto&& [i, out] :
         my_std::views::enumerate(checksums | views::drop(stride) | views::take(chunk_size))) {
      auto checksum{std::format("{}{:d}", msg, i_begin + stride + i)};
      for (int r{}; r < n_repeat + 1; ++r) {
        checksum = md5::hexdigest(md5::sum(checksum));
      }
      out = checksum;
    }
  }
};

void fill_checksums_parallel(std::string_view msg, const auto i, const auto stretch_count) {
  for (auto&& [t, th] : my_std::views::enumerate(threads)) {
    th = std::thread(md5sum_repeated{}, msg, i, stretch_count, t * chunk_size);
  }
  ranges::for_each(threads, [](auto& th) { th.join(); });
}

// TODO ranges::adjacent
// TODO template index sequence
constexpr decltype(auto) window5(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3), views::drop(r, 4));
}

auto stretch_search(std::string_view msg, const int stretch_count = 0) {
  std::set<int> keys;

  std::unordered_map<char, std::unordered_set<int>> repeat3;
  int latest_r3{};

  for (int i{}; i < 5'000'000; ++i) {
    if (i % checksums.size() == 0) {
      fill_checksums_parallel(msg, i, stretch_count);
    }
    bool found_r3{false};
    for (const auto& [a, b, c, d, e] : window5(checksums.at(i % checksums.size()))) {
      if (!found_r3) {
        if (a == b && b == c) {
          repeat3[a].insert(i);
          found_r3 = true;
        } else if (b == c && c == d) {
          repeat3[b].insert(i);
          found_r3 = true;
        } else if (c == d && d == e) {
          repeat3[c].insert(i);
          found_r3 = true;
        }
      }
      if (found_r3 && keys.size() < 64) {
        latest_r3 = i;
      }
      if (a == b && b == c && c == d && d == e) {
        for (const auto& i_prev : repeat3[a]) {
          if (const auto dist{i - i_prev}; 0 < dist && dist <= 1000) {
            keys.insert(i_prev);
          }
        }
      }
    }
    if (keys.size() > 63 && i - latest_r3 > 1000) {
      break;
    }
  }

  if (const auto key{ranges::begin(views::drop(keys, 63))}; key != keys.end()) {
    return *key;
  }
  throw std::runtime_error("search space exhausted with no key found");
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string msg;
  std::cin >> msg;

  const auto part1{stretch_search(msg)};
  const auto part2{stretch_search(msg, 2016)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
