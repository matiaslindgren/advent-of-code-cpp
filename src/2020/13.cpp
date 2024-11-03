#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

int find_part1(int earliest, const auto& ids) {
  int res{};
  int min{std::numeric_limits<int>::max()};
  for (auto id : views::values(ids)) {
    if (auto x{id - earliest % id}; x < min) {
      min = x;
      res = id * x;
    }
  }
  return res;
}

using Int = long long;

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

constexpr auto product{std::bind_back(ranges::fold_left, Int{1}, std::multiplies{})};

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

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int earliest{}; is >> earliest) {
    std::vector<std::pair<int, int>> ids;
    if (std::string line; is >> std::ws and std::getline(is, line)) {
      ranges::replace(line, ',', ' ');
      std::istringstream ls{line};
      for (int i{}; ls >> line; ++i) {
        if (line != "x") {
          std::istringstream is{line};
          if (int x{}; is >> x) {
            ids.emplace_back(x - i, x);
          } else {
            throw std::runtime_error(std::format("invalid value '{}'", line));
          }
        }
      }
    }
    return std::pair{earliest, ids};
  }
  throw std::runtime_error("failed parsing input");
}

int main() {
  const auto [earliest, ids]{parse_input("/dev/stdin")};

  const auto part1{find_part1(earliest, ids)};
  const auto part2{find_part2(ids)};

  std::println("{} {}", part1, part2);

  return 0;
}
