#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

struct Map {
  std::unordered_map<Vec2, int> energies;
  static constexpr std::array adjacencies{
      Vec2(-1, -1),
      Vec2(-1, 0),
      Vec2(-1, 1),
      Vec2(0, -1),
      Vec2(0, 1),
      Vec2(1, -1),
      Vec2(1, 0),
      Vec2(1, 1),
  };

  void step() {
    for (int& energy : energies | views::values) {
      energy += 1;
    }
  }

  auto adjacent(Vec2 p) const {
    return adjacencies | views::transform([=](Vec2 d) { return p + d; })
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
  long part1{}, part2{};
  for (auto n_flashes{0UZ}; n_flashes < m.energies.size();) {
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

std::istream& operator>>(std::istream& is, Map& map) {
  Vec2 p;
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (line.size() != 10) {
      throw std::runtime_error("every row must be of length 10");
    }
    p.x() = 0;
    for (char ch : line) {
      switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          map.energies[p] = ch - '0';
          p.x() += 1;
          continue;
      }
      throw std::runtime_error("all non-whitespace input must be digits");
    }
  }
  if (p.y() != 10) {
    throw std::runtime_error("there should be exactly 10 rows");
  }
  if (not is and not is.eof()) {
    throw std::runtime_error("unknown error while parsing map");
  }
  return is;
}

int main() {
  std::ios::sync_with_stdio(false);
  Map m;
  std::cin >> m;
  const auto [part1, part2]{search(m)};
  std::println("{} {}", part1, part2);
  return 0;
}
