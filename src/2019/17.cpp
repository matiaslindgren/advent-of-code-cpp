import std;
import aoc;
import intcode;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using intcode::IntCode;
using Vec2 = aoc::Vec2<int>;

inline constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& program) {
  Vec2 vacuum;
  std::unordered_set<Vec2> tiles;
  {
    Vec2 pos;
    for (IntCode ic(program); not ic.is_done();) {
      ic.run_until_output();
      if (const auto out{ic.pop_output()}) {
        switch (out.value()) {
          case '.': {
            pos.x() += 1;
          } break;
          case '^': {
            vacuum = pos;
            [[fallthrough]];
          }
          case '#': {
            tiles.insert(pos);
            pos.x() += 1;
          } break;
          case '\n': {
            pos.x() = 0;
            pos.y() += 1;
          } break;
          default:
            throw std::runtime_error("unexpected output");
        }
      }
    }
  }
  std::unordered_set<Vec2> intersections;
  {
    std::unordered_set<Vec2> visited;
    for (std::deque q{vacuum}; not q.empty(); q.pop_front()) {
      Vec2 p{q.front()};
      if (auto&& [_, unseen]{visited.insert(p)}; not unseen) {
        continue;
      }
      if (not tiles.contains(p)) {
        continue;
      }
      if (ranges::all_of(p.adjacent(), [&](auto&& adj) { return tiles.contains(adj); })) {
        intersections.insert(p);
      }
      q.append_range(p.adjacent());
    }
  }
  return sum(views::transform(intersections, [](Vec2 p) { return p.x() * p.y(); }));
}

auto find_part2(const auto& program) {
  using std::operator""sv;
  IntCode ic(program);
  ic.store(0, 2, intcode::Mode::address);
  ic.push_input("A,B,A,C,C,A,B,C,B,B\n"sv);
  ic.push_input("L,8,R,10,L,8,R,8\n"sv);
  ic.push_input("L,12,R,8,R,8\n"sv);
  ic.push_input("L,8,R,6,R,6,R,10,L,8\n"sv);
  ic.push_input("n\n"sv);
  ic.run_to_end();
  return ic.pop_output().value();
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};

  const auto part1{find_part1(program)};
  const auto part2{find_part2(program)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
