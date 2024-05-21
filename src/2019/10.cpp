#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;
using Points = std::vector<Vec2>;

Points parse_asteroids(std::istream& is) {
  Points asteroids;
  {
    auto width{0UZ};
    Vec2 pos;
    for (std::string line; std::getline(is, line) and not line.empty(); pos.y() += 1) {
      if (not width) {
        width = line.size();
      } else if (line.size() != width) {
        throw std::runtime_error("every line must be of same width");
      }
      pos.x() = 0;
      for (char ch : line) {
        if (ch == '#') {
          asteroids.push_back(pos);
        } else if (ch != '.') {
          throw std::runtime_error("every tile must be . or #");
        }
        pos.x() += 1;
      }
    }
  }
  if (is.eof()) {
    return asteroids;
  }
  throw std::runtime_error("failed parsing grid");
}

double angle(const Vec2& p1, const Vec2& p2) {
  const auto pi{std::numbers::pi_v<double>};
  const Vec2 d{p1 - p2};
  double a{std::atan2(d.y(), d.x()) - pi / 2.0};
  return a < 0 ? a + 2.0 * pi : a;
}

auto search(Points asteroids) {
  Vec2 laser;
  auto max_seen{0UZ};

  for (const auto& p1 : asteroids) {
    std::unordered_set<Vec2> seen;
    for (const auto& p2 : asteroids) {
      if (p1 != p2) {
        const Vec2 d{p2 - p1};
        if (const auto gcd{std::gcd(d.y(), d.x())}) {
          seen.insert(p1 + d / Vec2(gcd, gcd));
        }
      }
    }
    if (seen.size() > max_seen) {
      laser = p1;
      max_seen = seen.size();
    }
  }

  ranges::sort(asteroids, {}, [laser](const Vec2& target) {
    return std::tuple{angle(laser, target), laser.distance(target)};
  });

  int destroyed{};
  double prev_angle{-1};

  for (const Vec2& target : asteroids) {
    if (auto a{angle(laser, target)}; a == prev_angle) {
      continue;
    } else {
      prev_angle = a;
    }
    if (++destroyed == 200) {
      return std::pair{max_seen, target.x() * 100 + target.y()};
    }
  }

  throw std::runtime_error("destroyed all asteroids, couldn't find 200th");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto [part1, part2]{search(parse_asteroids(input))};
  std::println("{} {}", part1, part2);
  return 0;
}
