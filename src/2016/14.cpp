import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

// TODO ranges::adjacent
// TODO template index sequence
constexpr decltype(auto) window5(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3), views::drop(r, 4));
}

auto stretch_search(std::string_view msg, const int stretch_count = 0) {
  using std::operator""s;

  std::unordered_map<char, std::set<int>> r3s, r5s;
  std::set<int> keys;

  for (int i{}; keys.size() < 70 && i < 1'000'000; ++i) {
    auto checksum{std::format("{}{:d}", msg, i)};
    for (int s{}; s < stretch_count + 1; ++s) {
      checksum = md5::hexdigest(md5::sum(checksum));
    }
    bool r3{false};
    for (const auto [ch0, ch1, ch2, ch3, ch4] : window5(checksum)) {
      if (!r3) {
        if (ch0 == ch1 && ch1 == ch2) {
          r3s[ch0].insert(i);
          r3 = true;
        } else if (ch1 == ch2 && ch2 == ch3) {
          r3s[ch1].insert(i);
          r3 = true;
        } else if (ch2 == ch3 && ch3 == ch4) {
          r3s[ch2].insert(i);
          r3 = true;
        }
      }
      if (ch0 == ch1 && ch1 == ch2 && ch2 == ch3 && ch3 == ch4) {
        r5s[ch0].insert(i);
      }
    }
    for (const auto& ch : r5s | views::keys) {
      for (const auto& r5_i : r5s[ch]) {
        for (const auto& r3_i : r3s[ch]) {
          if (const auto dist{r5_i - r3_i}; 0 < dist && dist <= 1000) {
            keys.insert(r3_i);
          }
        }
      }
    }
  }

  if (const auto key{ranges::begin(views::drop(keys, 63))}; key != keys.end()) {
    return *key;
  }

  return std::numeric_limits<int>::max();
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
