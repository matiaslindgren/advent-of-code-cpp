import std;
import aoc;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

bool is_open(auto h) {
  return 0xb <= h and h <= 0xf;
}

std::pair<std::string, std::string> search(const std::string& passcode) {
  using std::operator""s;

  std::string part1{}, part2{};

  for (std::deque q{std::tuple{0, 0, ""s}}; not q.empty(); q.pop_front()) {
    const auto& [x, y, path] = q.front();
    if (not(0 <= x and x <= 3 and 0 <= y and y <= 3)) {
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

  return {part1, part2};
}

int main() {
  std::ios::sync_with_stdio(false);

  std::string passcode;
  std::cin >> passcode;

  const auto [part1, part2] = search(passcode);

  std::print("{} {}\n", part1, part2.size());

  return 0;
}
