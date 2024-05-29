#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;
using Points = std::vector<Vec2>;

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
    if (auto a{angle(laser, target)}; a != prev_angle) {
      prev_angle = a;
      if (++destroyed == 200) {
        return std::pair{max_seen, target.x() * 100 + target.y()};
      }
    }
  }

  throw std::runtime_error("destroyed all asteroids, couldn't find 200th");
}

Points parse_asteroids(std::string_view path) {
  Points asteroids;
  auto width{0UZ};
  for (Vec2 pos; const std::string& line : aoc::slurp_lines(path)) {
    if (width == 0) {
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
    pos.y() += 1;
  }
  return asteroids;
}
int main() {
  const auto [part1, part2]{search(parse_asteroids("/dev/stdin"))};
  std::println("{} {}", part1, part2);
  return 0;
}
