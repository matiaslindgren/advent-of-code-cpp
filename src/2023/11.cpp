import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Point2D {
  using Int = std::size_t;
  Int x;
  Int y;
};

constexpr auto l1_norm(const Point2D& lhs, const Point2D& rhs) {
  const auto dx{std::max(lhs.x, rhs.x) - std::min(lhs.x, rhs.x)};
  const auto dy{std::max(lhs.y, rhs.y) - std::min(lhs.y, rhs.y)};
  return dx + dy;
}

using Points = std::vector<Point2D>;
using Ints = std::unordered_set<Point2D::Int>;

struct Space {
  Points galaxies;
  Ints galaxy_rows;
  Ints galaxy_cols;

  explicit Space() = default;
  explicit Space(const Points& input) : galaxies{input} {
    for (const auto& p : input) {
      galaxy_rows.insert(p.y);
      galaxy_cols.insert(p.x);
    }
  }

  auto distance_sum(const auto expansion) const {
    const auto n{galaxies.size()};
    return my_std::ranges::fold_left(
        views::iota(0uz, n * n) | views::transform([=, this](const auto& i) -> long {
          if (const auto [i1, i2]{std::lldiv(i, n)}; i1 < i2) {
            const auto p1{this->galaxies[i1]};
            const auto p2{this->galaxies[i2]};
            return l1_norm(p1, p2) + (expansion - 1) * count_expansions(p1, p2);
          }
          return {};
        }),
        0L,
        std::plus{}
    );
  }

 private:
  long count_expansions(const Point2D& src, const Point2D& dst) const {
    return count_expansions(src.x, dst.x, galaxy_cols)
           + count_expansions(src.y, dst.y, galaxy_rows);
  }

  long count_expansions(const auto src, const auto dst, const auto& has_galaxy) const {
    return ranges::count_if(
        views::iota(std::min(src, dst), std::max(src, dst) + 1),
        [&](const auto& i) { return !has_galaxy.contains(i); }
    );
  }
};

std::istream& operator>>(std::istream& is, Space& space) {
  Points galaxies;
  for (auto [ch, y, x] = std::make_tuple('\0', 0uz, 0uz); is.get(ch);) {
    switch (ch) {
      case '\n':
        x = 0;
        y += 1;
        break;
      case '#':
        galaxies.push_back(Point2D{x, y});
      case '.':
        x += 1;
        break;
      default:
        is.setstate(std::ios_base::failbit);
    }
  }
  if (is.eof()) {
    space = Space{galaxies};
    return is;
  }
  throw std::runtime_error("invalid input galaxy, must contain only # or .");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Space space;
  input >> space;

  const auto part1{space.distance_sum(2)};
  const auto part2{space.distance_sum(1'000'000)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
