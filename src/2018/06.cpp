import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Vec2 {
  int x{}, y{};

  Vec2 operator+(const Vec2& rhs) const {
    return {x + rhs.x, y + rhs.y};
  }
  auto distance(const Vec2& rhs) const {
    return std::abs(x - rhs.x) + std::abs(y - rhs.y);
  }
  auto operator<=>(const Vec2&) const = default;
};

template <>
struct std::hash<Vec2> {
  auto to_unsigned(const int a) const noexcept {
    return static_cast<std::size_t>(a + std::numeric_limits<int>::max() / 2);
  }
  std::size_t operator()(const Vec2& v) const noexcept {
    constexpr auto slot{std::numeric_limits<std::size_t>::digits / 2};
    return (to_unsigned(v.x) << slot) | to_unsigned(v.y);
  }
};

std::istream& operator>>(std::istream& is, Vec2& vec) {
  using aoc::skip;
  using std::operator""s;
  if (Vec2 v; is >> std::ws && is >> v.x && skip(is, ","s) && is >> v.y) {
    vec = v;
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Vec2");
}

constexpr auto intmin{std::numeric_limits<int>::min()};
constexpr auto intmax{std::numeric_limits<int>::max()};

constexpr auto max{std::__bind_back(my_std::ranges::fold_left, intmin, [](auto a, auto b) {
  return std::max(a, b);
})};
constexpr auto min{std::__bind_back(my_std::ranges::fold_left, intmax, [](auto a, auto b) {
  return std::min(a, b);
})};

auto find_grid_corners(const auto& points) {
  return my_std::ranges::fold_left(
      points,
      std::pair{Vec2{intmax, intmax}, Vec2{intmin, intmin}},
      [](const auto& corners, const auto& p) {
        auto [tl, br] = corners;
        return std::pair{
            Vec2{std::min(tl.x, p.x), std::min(tl.y, p.y)},
            Vec2{std::max(br.x, p.x), std::max(br.y, p.y)}
        };
      }
  );
}

auto find_areas(const auto& points) {
  const auto [top_left, bottom_right]{find_grid_corners(points)};

  const auto centers{my_std::views::enumerate(points, 1uz) | ranges::to<std::vector>()};
  std::unordered_map<Vec2, std::vector<std::size_t>> claims;
  std::unordered_map<Vec2, std::size_t> total_dist;

  for (Vec2 p{.y = top_left.y - 1}; p.y < bottom_right.y + 1; ++p.y) {
    for (p.x = top_left.x - 1; p.x < bottom_right.x + 1; ++p.x) {
      const auto& min_dist{
          min(views::transform(points, [&](const auto& center) { return center.distance(p); }))
      };
      for (auto [id, center] : centers) {
        const auto d{center.distance(p)};
        total_dist[p] += d;
        if (d == min_dist) {
          claims[p].push_back(id);
        }
      }
    }
  }

  const auto is_finite{[&](const auto& p) {
    const auto y_finite{top_left.y < p.y && p.y < bottom_right.y};
    const auto x_finite{top_left.x < p.x && p.x < bottom_right.x};
    return y_finite && x_finite;
  }};

  const auto count_area{[&](const auto& ic) {
    auto [id, center] = ic;
    const std::array adjacent{
        Vec2{-1, 0},
        Vec2{1, 0},
        Vec2{0, -1},
        Vec2{0, 1},
    };
    int area{};
    std::unordered_set<Vec2> seen;
    for (std::deque q{center}; !q.empty(); q.pop_front()) {
      const auto& p{q.front()};
      if (!is_finite(p)) {
        area = 0;
        break;
      }
      if (seen.insert(p).second && claims.contains(p)) {
        if (const auto& ids{claims.at(p)}; ids.size() == 1 && ids.front() == id) {
          area += 1;
          for (const auto& d : adjacent) {
            q.push_back(p + d);
          }
        }
      }
    }
    return area;
  }};

  return std::pair{
      max(views::transform(centers, count_area)),
      ranges::count_if(total_dist | views::values, [](auto d) { return d < 10000; })
  };
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto points{views::istream<Vec2>(input) | ranges::to<std::vector>()};

  const auto [part1, part2]{find_areas(points)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
