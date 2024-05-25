#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<unsigned long>;
using Points = std::vector<Vec2>;
using Ints = std::unordered_set<Vec2::value_type>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

struct Space {
  Points galaxies;
  Ints galaxy_rows;
  Ints galaxy_cols;

  explicit Space(const Points& input) : galaxies{input} {
    for (const Vec2& p : input) {
      galaxy_rows.insert(p.y());
      galaxy_cols.insert(p.x());
    }
  }

  [[nodiscard]]
  auto count_expansions(const auto src, const auto dst, const auto& has_galaxy) const {
    return ranges::count_if(
        views::iota(std::min(src, dst), std::max(src, dst) + 1),
        [&](const auto& i) { return not has_galaxy.contains(i); }
    );
  }

  [[nodiscard]]
  auto count_expansions(const Vec2& src, const Vec2& dst) const {
    return count_expansions(src.x(), dst.x(), galaxy_cols)
           + count_expansions(src.y(), dst.y(), galaxy_rows);
  }

  [[nodiscard]]
  auto distance_sum(long expansion) const {
    const auto n{galaxies.size()};
    return sum(views::transform(
        views::iota(0UZ, n * n),
        [n, expansion, this](const auto& i) -> Vec2::value_type {
          if (const auto [i1, i2]{std::lldiv(i, n)}; i1 < i2) {
            const Vec2& p1{galaxies.at(i1)};
            const Vec2& p2{galaxies.at(i2)};
            return (p1.max(p2) - p1.min(p2)).sum() + (expansion - 1) * count_expansions(p1, p2);
          }
          return 0;
        }
    ));
  }
};

Space parse_space(std::string_view path) {
  Points galaxies;
  Vec2 p;
  std::size_t width{};
  std::istringstream is{aoc::slurp_file(path)};
  for (std::string line; std::getline(is, line) and not line.empty(); p.y() += 1) {
    if (width == 0U) {
      width = line.size();
    } else if (width != line.size()) {
      throw std::runtime_error("every row must be of equal length");
    }
    p.x() = 0;
    for (char ch : line) {
      if (ch == '#') {
        galaxies.push_back(p);
      } else if (ch != '.') {
        throw std::runtime_error(std::format("unknown tile '{}'", ch));
      }
      p.x() += 1;
    }
  }
  return Space(galaxies);
}

int main() {
  const Space s{parse_space("/dev/stdin")};

  const auto part1{s.distance_sum(2)};
  const auto part2{s.distance_sum(1'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
