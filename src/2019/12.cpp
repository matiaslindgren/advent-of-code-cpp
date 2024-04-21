#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

using Vec3 = aoc::Vec3<int>;

struct Moon {
  Vec3 p, v;
};

using Moons = std::array<Moon, 4>;

template <std::size_t axis>
auto hash_axis(const Moons& moons) {
  auto state_str{my_std::ranges::fold_left(moons, ""s, [](auto&& s, const Moon& m) {
    return s + std::format("{}{}", m.p.get<axis>(), m.v.get<axis>());
  })};
  return std::hash<std::string>{}(state_str);
}

inline constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto search(Moons moons) {
  long part1{}, part2{};
  {
    std::array<std::unordered_set<std::size_t>, Vec3::ndim> states;
    std::array<long, Vec3::ndim> cycles;
    states.fill({});
    cycles.fill(0);

    for (auto step{0uz}; ranges::any_of(cycles, [](long c) { return c == 0; }); ++step) {
      if (step == 1'000) {
        part1 = sum(views::transform(moons, [](const Moon& m) {
          return m.p.distance(Vec3()) * m.v.distance(Vec3());
        }));
      }

      [&]<std::size_t... axis>(std::index_sequence<axis...>) {
        (
            [&] {
              if (not cycles[axis]) {
                auto hash{hash_axis<axis>(moons)};
                if (auto&& [_, unseen]{states[axis].insert(hash)}; not unseen) {
                  cycles[axis] = step;
                }
              }
            }(),
            ...
        );
      }(std::make_index_sequence<Vec3::ndim>{});

      for (auto&& [i, m1] : my_std::views::enumerate(moons)) {
        for (auto& m2 : views::drop(moons, i + 1)) {
          const Vec3 gravity{(m1.p - m2.p).signum()};
          m1.v -= gravity;
          m2.v += gravity;
        }
      }

      ranges::for_each(moons, [](Moon& m) { m.p += m.v; });
    }

    part2 = std::lcm(cycles[0], std::lcm(cycles[1], cycles[2]));
  }
  return std::pair{part1, part2};
}

std::istream& operator>>(std::istream& is, Moon& m) {
  using aoc::skip;
  if (int x, y, z; is >> std::ws >> skip("<x="s) >> x >> skip(","s, "y="s) >> y >> skip(","s, "z="s)
                   >> z >> skip(">"s)) {
    m = Moon{.p = Vec3(x, y, z), .v = Vec3()};
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Moon");
}

Moons parse_system(const std::string path) {
  std::istringstream input{aoc::slurp_file(path)};
  auto moons{views::istream<Moon>(input) | ranges::to<std::vector>()};
  if (input.eof() and moons.size() == 4) {
    return {moons[0], moons[1], moons[2], moons[3]};
  }
  throw std::runtime_error("invalid input, failed parsing system of 4 moons");
}

int main() {
  const auto [part1, part2]{search(parse_system("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
