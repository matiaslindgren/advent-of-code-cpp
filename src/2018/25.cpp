import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Vec4 {
  int x, y, z, w;

  auto operator<=>(const Vec4&) const = default;

  int distance(const Vec4& rhs) const {
    return std::abs(x - rhs.x) + std::abs(y - rhs.y) + std::abs(z - rhs.z) + std::abs(w - rhs.w);
  }
};

template <>
struct std::hash<Vec4> {
  std::size_t operator()(const Vec4& v) const noexcept {
    constexpr int n{10};
    return (v.x + n) * n * n * n + (v.y + n) * n * n + (v.z + n) * n + v.w + n;
  }
};

std::istream& operator>>(std::istream& is, Vec4& vec) {
  if (Vec4 v; is >> v.x >> skip(","s) >> v.y >> skip(","s) >> v.z >> skip(","s) >> v.w) {
    vec = v;
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Vec4");
}

auto find_part1(const auto& points) {
  auto adjacent{
      views::transform(
          points,
          [](auto&& p) {
            return std::pair{p, std::vector<Vec4>{}};
          }
      )
      | ranges::to<std::unordered_map>()
  };

  for (auto&& [p1, p2] : my_std::views::cartesian_product(points, points)) {
    if (const auto d{p1.distance(p2)}; 0 < d and d <= 3) {
      adjacent.at(p1).push_back(p2);
    }
  }

  const auto ids{
      views::zip(points, views::iota(0uz, points.size())) | ranges::to<std::unordered_map>()
  };

  constexpr auto root_sentinel{std::numeric_limits<std::size_t>::max()};
  std::vector<std::size_t> parents(points.size(), root_sentinel);

  const auto union_find_merge{[&](auto&& p1, auto&& p2) {
    auto root{ids.at(p1)};
    while (parents.at(root) != root_sentinel) {
      root = parents.at(root);
    }
    for (auto parent{ids.at(p2)}; parent != root_sentinel;) {
      const auto next{parents.at(parent)};
      if (parent != root) {
        parents[parent] = root;
      }
      parent = next;
    }
  }};

  for (auto&& [p1, adj] : adjacent) {
    for (auto&& p2 : adj) {
      union_find_merge(p1, p2);
    }
  }

  return ranges::count(parents, root_sentinel);
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto points{views::istream<Vec4>(input) | ranges::to<std::vector>()};
  std::print("{}\n", find_part1(points));
  return 0;
}
