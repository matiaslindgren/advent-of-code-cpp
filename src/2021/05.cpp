#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;

std::vector<int> as_points(const int a, const int b) {
  std::vector<int> v;
  if (a < b) {
    v.append_range(views::iota(a, b + 1));
  } else {
    v.append_range(views::iota(b, a + 1) | views::reverse);
  }
  return v;
}

std::vector<Vec2> as_points(const Vec2& a, const Vec2& b) {
  return views::zip(as_points(a.x(), b.x()), as_points(a.y(), b.y()))
         | views::transform([](auto&& xy) { return Vec2(xy.first, xy.second); })
         | ranges::to<std::vector>();
}

struct Segment {
  Vec2 begin;
  Vec2 end;

  bool is_diagonal() const {
    return begin.x() != end.x() and begin.y() != end.y();
  }
};

std::istream& operator>>(std::istream& is, Segment& segment) {
  using aoc::skip;
  using std::operator""s;

  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (int x1, y1; ls >> x1 >> skip(","s) >> y1) {
      if (ls >> std::ws >> skip("->"s)) {
        if (int x2, y2; ls >> x2 >> skip(","s) >> y2) {
          segment = {Vec2(x1, y1), Vec2(x2, y2)};
        }
      }
    }
    if (not(ls >> std::ws).eof()) {
      is.setstate(std::ios_base::failbit);
    }
  }

  if (not is and not is.eof()) {
    throw std::runtime_error("failed parsing segment");
  }

  return is;
}

auto search(const auto& segments) {
  std::unordered_map<Vec2, int> counts;

  for (const Segment& s : segments) {
    if (not s.is_diagonal()) {
      const Vec2 min{s.begin.min(s.end)};
      const Vec2 max{s.begin.max(s.end)};
      for (Vec2 p{min}; p.y() <= max.y(); ++p.y()) {
        for (p.x() = min.x(); p.x() <= max.x(); ++p.x()) {
          counts[p] += 1;
        }
      }
    }
  }
  auto part1{ranges::count_if(counts | views::values, [](int n) { return n > 1; })};

  for (const Segment& s : segments) {
    if (s.is_diagonal()) {
      for (const Vec2& p : as_points(s.begin, s.end)) {
        counts[p] += 1;
      }
    }
  }
  auto part2{ranges::count_if(counts | views::values, [](int n) { return n > 1; })};

  return std::pair{part1, part2};
}

int main() {
  const auto segments{aoc::slurp<Segment>("/dev/stdin")};
  const auto [part1, part2]{search(segments)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
