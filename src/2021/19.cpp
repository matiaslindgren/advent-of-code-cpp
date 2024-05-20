#include "aoc.hpp"
#include "my_std.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

using Vec3 = ndvec::vec3<int>;
using Vec3s = std::vector<Vec3>;
using BeaconMap = std::unordered_map<Vec3, std::pair<Vec3s, Vec3s>>;

struct Scanner {
  int id;
  Vec3 center;
  Vec3s beacons;

  auto beacon_pairs() const {
    return my_std::views::cartesian_product(beacons, beacons)
           | views::filter(my_std::apply_fn(std::not_equal_to<Vec3>{}));
  }

  BeaconMap make_beacon_map(BeaconMap map = {}) const {
    for (auto&& [b1, b2] : beacon_pairs()) {
      Vec3 delta{b1 - b2};
      map[delta].first.push_back(b1);
      map[delta].second.push_back(b2);
    }
    return map;
  }

  void rotate90(int axis) {
    ranges::for_each(beacons, [axis](Vec3& b) {
      switch (axis) {
        case 0:
          b.y() = -std::exchange(b.z(), b.y());
          return;
        case 1:
          b.z() = -std::exchange(b.x(), b.z());
          return;
        case 2:
          b.x() = -std::exchange(b.y(), b.x());
          return;
      }
      throw std::runtime_error("unknown rotation axis");
    });
  }

  bool rotate_until_overlap(const BeaconMap& b2b) {
    for (int axis{}; axis < 3; ++axis) {
      for (int i{}; i < 4; ++i) {
        for (int j{}; j < 2; ++j) {
          const auto n_overlap{ranges::count_if(beacon_pairs(), [&b2b](auto b1b2) {
            auto [b1, b2]{b1b2};
            return b2b.contains(b1 - b2);
          })};
          if (n_overlap == 132) {
            return true;
          }
          for (int k{}; k < 2; ++k) {
            rotate90((axis + 2) % 3);
          }
        }
        rotate90(axis);
      }
      rotate90((axis + 2) % 3);
    }
    return false;
  }

  void center_on(BeaconMap b2b) {
    b2b = make_beacon_map(b2b);
    for (auto [b1, _] : b2b | views::values) {
      if (b1.size() > 1) {
        center = b1[0] - b1[1];
        break;
      }
    }
    ranges::for_each(beacons, [this](Vec3& b) { b += this->center; });
  }
};

Scanner find_overlapping(const auto& locked, const auto& available) {
  for (const Scanner& s1 : locked) {
    BeaconMap b2b{s1.make_beacon_map()};
    for (Scanner s2 : available) {
      if (s2.rotate_until_overlap(b2b)) {
        s2.center_on(b2b);
        return s2;
      }
    }
  }
  throw std::runtime_error("failed to find overlapping scanner");
}

auto recenter(auto scanners) {
  if (scanners.empty()) {
    std::runtime_error("cannot recenter empty scanner list");
  }

  std::vector<Scanner> locked{{scanners.front()}};
  scanners.erase(scanners.begin());

  while (not scanners.empty()) {
    auto overlapping{find_overlapping(locked, scanners)};
    locked.push_back(overlapping);
    std::erase_if(scanners, [&](const Scanner& s) { return s.id == overlapping.id; });
  }

  return locked;
}

std::istream& operator>>(std::istream& is, Scanner& s) {
  if (int id; is >> skip("--- scanner"s) >> id >> std::ws >> skip("---"s)) {
    s.id = id;
    for (Vec3 p; is >> p;) {
      s.beacons.push_back(p);
    }
  }
  return is;
}

auto find_part1(const auto& scanners) {
  auto unique_beacons{
      scanners | views::transform([](Scanner s) { return s.beacons; }) | views::join
      | ranges::to<std::set>()
  };
  return unique_beacons.size();
}

auto find_part2(const auto& scanners) {
  return ranges::max(
      my_std::views::cartesian_product(scanners, scanners) | views::transform([](auto&& s1s2) {
        auto [s1, s2]{s1s2};
        return s2.center.distance(s1.center);
      })
  );
}

auto parse_scanners(std::string path) {
  std::vector<Scanner> scanners;
  {
    auto input{aoc::slurp_file(path)};
    ranges::replace(input, ',', ' ');
    for (auto section : input | views::split("\n\n"s)) {
      std::istringstream is{ranges::to<std::string>(section)};
      if (Scanner s; (is >> s >> std::ws).eof()) {
        scanners.push_back(s);
      } else {
        throw std::runtime_error("failed parsing scanner");
      }
    }
  }
  if (scanners.empty()) {
    throw std::runtime_error("input should contain sections separated by 2 newlines");
  }
  return scanners;
}

int main() {
  auto scanners{parse_scanners("/dev/stdin")};
  scanners = recenter(scanners);

  const auto part1{find_part1(scanners)};
  const auto part2{find_part2(scanners)};

  std::println("{} {}", part1, part2);

  return 0;
}
