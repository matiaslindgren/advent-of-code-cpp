#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = ndvec::vec3<long>;
using aoc::skip;
using std::operator""s;

struct Particle {
  Vec3 p;
  Vec3 v;
  Vec3 a;
  int id{};

  [[nodiscard]]
  Particle at_time(long t) const {
    Vec3 t1(t, t, t);
    Vec3 t2(t1 * t1);
    return {
        p + v * t1 + a * t2,
        v + a * t1,
        a,
    };
  }
};

std::pair<long, long> solve_quadratic(const auto a, const auto b, const auto c) {
  if (a) {
    const auto inv2a{1 / (2.0 * a)};
    if (const auto bb4ac{b * b - 4 * a * c}; bb4ac > 0) {
      return {
          std::round((-b + std::sqrt(bb4ac)) * inv2a),
          std::round((-b - std::sqrt(bb4ac)) * inv2a)
      };
    }
    return {std::round(-b * inv2a), -1};
  }
  if (b) {
    return {std::round(1.0 * -c / b), -1};
  }
  return {-1, -1};
}

auto find_part1(const auto& particles) {
  const auto closest{ranges::min_element(particles, ranges::less{}, [](const auto& particle) {
    return particle.at_time(10'000'000).p.distance(Vec3{});
  })};
  if (closest != particles.end()) {
    return closest->id;
  }
  throw std::runtime_error("cannot find part1");
}

auto find_part2(const auto& particles) {
  std::unordered_map<int, std::vector<std::pair<int, int>>> collisions;

  for (auto&& [p1, p2] : my_std::views::cartesian_product(particles, particles)) {
    if (p1.id == p2.id) {
      continue;
    }
    const auto a{p1.a - p2.a};
    const auto b{(p1.v - p2.v) * Vec3(2, 2, 2) + a};
    const auto c{(p1.p - p2.p) * Vec3(2, 2, 2)};
    const auto [tx0, tx1]{solve_quadratic(a.x(), b.x(), c.x())};
    const auto [ty0, ty1]{solve_quadratic(a.y(), b.y(), c.y())};
    const auto [tz0, tz1]{solve_quadratic(a.z(), b.z(), c.z())};
    if (tx0 >= 0 and tx0 == ty0 and ty0 == tz0) {
      collisions[tx0].emplace_back(p1.id, p2.id);
    }
    if (tx1 >= 0 and tx1 == ty1 and ty1 == tz1) {
      collisions[tx1].emplace_back(p1.id, p2.id);
    }
  }

  auto collision_times{collisions | views::keys | ranges::to<std::vector>()};
  ranges::sort(collision_times);

  std::unordered_set<int> destroyed;
  for (const auto& t : collision_times) {
    std::vector<int> d;
    for (auto&& [p1, p2] : collisions.at(t)) {
      if (not destroyed.contains(p1) and not destroyed.contains(p2)) {
        d.push_back(p1);
        d.push_back(p2);
      }
    }
    destroyed.insert_range(d);
  }

  return particles.size() - destroyed.size();
}

std::istream& operator>>(std::istream& is, Particle& particle) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (Vec3 p, v, a; ls >> std::ws >> skip("p=<"s) >> p >> skip(">"s) >> std::ws >> skip("v=<"s)
                      >> v >> skip(">"s) >> std::ws >> skip("a=<"s) >> a >> skip(">"s)) {
      particle = {p, v, a};
    } else {
      throw std::runtime_error(std::format("failed parsing 3 vectors from line '{}'", line));
    }
  }
  return is;
}

int main() {
  const auto particles{
      my_std::views::enumerate(aoc::parse_items<Particle>("/dev/stdin"))
      | views::transform(my_std::apply_fn([](auto i, Particle p) {
          p.id = i;
          return p;
        }))
      | ranges::to<std::vector>()
  };

  const auto part1{find_part1(particles)};
  const auto part2{find_part2(particles)};

  std::println("{} {}", part1, part2);

  return 0;
}
