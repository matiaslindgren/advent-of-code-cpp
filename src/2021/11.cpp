#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Map {
  std::unordered_map<Vec2, int> energies;

  void step() {
    for (int& energy : energies | views::values) {
      energy += 1;
    }
  }

  [[nodiscard]]
  auto adjacent(Vec2 p) const {
    return std::array{
               Vec2(-1, -1),
               Vec2(-1, 0),
               Vec2(-1, 1),
               Vec2(0, -1),
               Vec2(0, 1),
               Vec2(1, -1),
               Vec2(1, 0),
               Vec2(1, 1),
           }
           | views::transform([=](Vec2 d) { return p + d; })
           | views::filter([this](Vec2 adj) { return this->energies.contains(adj); });
  }

  bool flash_all() {
    Map m{*this};
    bool flashed{false};
    for (auto&& [p, energy] : energies) {
      if (energy > 9) {
        flashed = true;
        m.energies[p] = -1;
        for (Vec2 adj : adjacent(p)) {
          if (m.energies[adj] >= 0) {
            m.energies[adj] += 1;
          }
        }
      }
    }
    if (flashed) {
      energies = m.energies;
    }
    return flashed;
  }
};

auto search(Map m) {
  long part1{};
  long part2{};
  for (long n_flashes{}; n_flashes < m.energies.size();) {
    n_flashes = 0;
    for (m.step(); m.flash_all();) {
    }
    for (int& energy : m.energies | views::values) {
      if (energy < 0) {
        energy = 0;
        n_flashes += 1;
      }
    }
    part1 += n_flashes;
    part2 += 1;
  }
  return std::pair{part1, part2};
}

Map parse_map(std::string_view path) {
  Map map;
  Vec2 p;
  for (const std::string& line : aoc::slurp_lines(path)) {
    if (line.size() != 10) {
      throw std::runtime_error("every row must be of length 10");
    }
    for (p.x() = 0; char ch : line) {
      if (not aoc::is_digit(ch)) {
        throw std::runtime_error("all non-whitespace input must be digits");
      }
      map.energies[p] = ch - '0';
      p.x() += 1;
    }
    p.y() += 1;
  }
  if (p.y() != 10) {
    throw std::runtime_error("there should be exactly 10 rows");
  }
  return map;
}

int main() {
  const Map m{parse_map("/dev/stdin")};
  const auto [part1, part2]{search(m)};
  std::println("{} {}", part1, part2);
  return 0;
}
