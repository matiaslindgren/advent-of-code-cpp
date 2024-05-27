#include "aoc.hpp"
#include "md5.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

bool is_open(auto h) {
  return 0xb <= h and h <= 0xf;
}

auto search(const std::string& passcode) {
  std::string part1{};
  std::string part2{};
  for (std::deque q{std::tuple{0, 0, ""s}}; not q.empty(); q.pop_front()) {
    auto [x, y, path]{q.front()};
    if (std::min(x, y) < 0 or std::max(x, y) > 3) {
      continue;
    }
    if (x == 3 and y == 3) {
      if (part1.empty() or path.size() < part1.size()) {
        part1 = path;
      }
      if (path.size() > part2.size()) {
        part2 = path;
      }
      continue;
    }
    const auto hash{md5::sum32bit(passcode + path) >> 16};
    if (is_open((hash >> 12) & 0xf)) {
      q.emplace_back(x, y - 1, path + "U"s);
    }
    if (is_open((hash >> 8) & 0xf)) {
      q.emplace_back(x, y + 1, path + "D"s);
    }
    if (is_open((hash >> 4) & 0xf)) {
      q.emplace_back(x - 1, y, path + "L"s);
    }
    if (is_open((hash >> 0) & 0xf)) {
      q.emplace_back(x + 1, y, path + "R"s);
    }
  }
  return std::pair{part1, part2};
}

int main() {
  std::ios::sync_with_stdio(false);
  if (std::string passcode; std::cin >> passcode and ranges::all_of(passcode, aoc::is_lower)) {
    const auto [part1, part2]{search(passcode)};
    std::println("{} {}", part1, part2.size());
    return 0;
  }
  throw std::runtime_error("failed parsing ascii input password");
}
