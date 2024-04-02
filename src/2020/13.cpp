import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

int find_part1(int earliest, const auto& ids) {
  int res{}, min{std::numeric_limits<int>::max()};
  for (auto id : views::values(ids)) {
    if (auto x{id - earliest % id}; x < min) {
      min = x;
      res = id * x;
    }
  }
  return res;
}

using Int = __int128;

Int powmod(Int base, Int exp, Int mod) {
  // https://en.wikipedia.org/wiki/Modular_exponentiation
  // accessed 2024-03-28
  if (mod == 1) {
    return Int{};
  }
  Int res{1};
  for (base %= mod; exp > 0; exp /= 2) {
    if (exp % 2 == 1) {
      res = (res * base) % mod;
    }
    base = (base * base) % mod;
  }
  return res;
}

constexpr auto product{std::__bind_back(my_std::ranges::fold_left, Int{1}, std::multiplies{})};

long find_part2(const auto& ids) {
  // https://www.reddit.com/r/adventofcode/comments/kc4njx/comment/gfnbyg2
  // accessed 2024-04-02
  const auto m{product(views::values(ids))};
  Int sum{};
  for (auto [cid, id] : ids) {
    const auto a{m / id};
    const auto b{cid * a * powmod(a, id - 2, id)};
    sum = (m + (sum + b) % m) % m;
  }
  return sum;
}

auto parse_input() {
  std::ios::sync_with_stdio(false);

  int earliest;
  std::cin >> earliest;

  std::vector<std::pair<int, int>> ids;

  if (std::string line; std::getline(std::cin >> std::ws, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    for (int i{}; ls >> line; ++i) {
      if (line != "x") {
        std::istringstream is{line};
        int x;
        is >> x;
        ids.emplace_back(x - i, x);
      }
    }
  }

  return std::pair{earliest, ids};
}

int main() {
  const auto [earliest, ids]{parse_input()};

  const auto part1{find_part1(earliest, ids)};
  const auto part2{find_part2(ids)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
