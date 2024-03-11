import std;
import aoc;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::Vec2;

struct Grid {
  std::vector<bool> is_asteroid;
  std::size_t width{};

  auto asteroids() const {
    return views::iota(0uz, is_asteroid.size())
           | views::filter([this](auto i) { return is_asteroid.at(i); })
           | views::transform([this](int i) {
               auto [y, x]{std::div(i, width)};
               return Vec2{y, x};
             });
  }
};

Grid parse_grid(std::istream& is) {
  Grid g;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    if (not g.width) {
      g.width = line.size();
    } else if (line.size() != g.width) {
      throw std::runtime_error("every line must be of same width");
    }
    for (char ch : line) {
      if (ch == '#' or ch == '.') {
        g.is_asteroid.push_back(ch == '#');
      } else {
        throw std::runtime_error("every tile must be . or #");
      }
    }
  }
  if (is.eof()) {
    return g;
  }
  throw std::runtime_error("failed parsing grid");
}

double angle(const Vec2& p1, const Vec2& p2) {
  const auto pi{std::numbers::pi_v<double>};
  const Vec2 d{p1 - p2};
  double a{std::atan2(d.y, d.x) - pi / 2.0};
  return a < 0 ? a + 2.0 * pi : a;
}

auto search(const Grid& grid) {
  Vec2 laser;
  auto max_seen{0uz};

  for (auto p1 : grid.asteroids()) {
    std::unordered_set<Vec2> seen;
    for (auto p2 : grid.asteroids()) {
      if (p1 == p2) {
        continue;
      }
      const Vec2 d{p2 - p1};
      if (const auto gcd{std::gcd(d.y, d.x)}) {
        seen.insert(p1 + d / gcd);
      }
    }
    if (seen.size() > max_seen) {
      laser = p1;
      max_seen = seen.size();
    }
  }

  auto targets{grid.asteroids() | ranges::to<std::vector>()};
  ranges::sort(targets, {}, [laser](const Vec2& target) {
    return std::tuple{angle(laser, target), laser.distance(target)};
  });

  std::unordered_set<Vec2> destroyed;
  double prev_angle{-1};

  for (const Vec2& target : targets) {
    if (destroyed.contains(target)) {
      continue;
    }
    if (auto a{angle(laser, target)}; a == prev_angle) {
      continue;
    } else {
      prev_angle = a;
    }
    destroyed.insert(target);
    if (destroyed.size() == 200) {
      return std::pair{max_seen, target.x * 100 + target.y};
    }
  }

  throw std::runtime_error("destroyed all asteroids, couldn't find 200th");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const Grid grid{parse_grid(input)};

  const auto [part1, part2]{search(grid)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
