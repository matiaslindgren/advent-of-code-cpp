import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = aoc::Vec3<int>;

struct Brick {
  Vec3 begin, end;
  std::size_t index{};

  bool intersects(const Brick& rhs) const {
    const auto x{begin.x() < rhs.end.x() and rhs.begin.x() < end.x()};
    const auto y{begin.y() < rhs.end.y() and rhs.begin.y() < end.y()};
    const auto z{begin.z() < rhs.end.z() and rhs.begin.z() < end.z()};
    return x and y and z;
  }
};

struct SupportGraph {
  using Bricks = std::vector<Brick>;
  using Support = std::vector<std::unordered_set<std::size_t>>;

  Bricks bricks;
  Support supporting;
  Support supported_by;

  explicit SupportGraph(const Bricks& input)
      : bricks{input}, supporting(input.size()), supported_by(input.size()) {
    ranges::sort(bricks, ranges::less{}, [](const auto& b) { return b.begin.z(); });

    auto brick_count{0uz};
    for (auto curr{bricks.begin()}; curr != bricks.end(); ++curr) {
      curr->index = brick_count++;

      auto b1{*curr};
      const auto intersects_b1{[&b1](const auto& b2) { return b1.intersects(b2); }};

      while (b1.begin.z() > 0 and not ranges::any_of(bricks.begin(), curr, intersects_b1)) {
        *curr = b1;
        b1.begin.z() -= 1;
        b1.end.z() -= 1;
      }

      for (const auto& b2 : ranges::subrange(bricks.begin(), curr) | views::filter(intersects_b1)) {
        supporting[b2.index].insert(b1.index);
        supported_by[b1.index].insert(b2.index);
      }
    }
  }
};

std::istream& operator>>(std::istream& is, Brick& brick) {
  using aoc::skip;
  using std::operator""s;
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    if (Vec3 begin, end; ls >> begin >> skip("~"s) >> end) {
      brick = {begin, end + Vec3(1, 1, 1)};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Brick");
}

auto find_part1(const SupportGraph& sg) {
  return ranges::count_if(sg.bricks, [&sg](const Brick& b1) {
    return ranges::all_of(sg.supporting.at(b1.index), [&sg](const auto& b2) {
      return sg.supported_by.at(b2).size() != 1;
    });
  });
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto find_part2(const SupportGraph& sg) {
  return sum(views::transform(sg.bricks, [&sg](const Brick& b) {
    int n{};
    std::vector<bool> falling(sg.bricks.size());
    falling.at(b.index) = true;
    for (std::deque q{b.index}; not q.empty(); q.pop_front()) {
      for (const auto above : sg.supporting.at(q.front())) {
        const auto support_count{ranges::count_if(
            sg.supported_by.at(above),
            [&falling](const auto& below) { return not falling.at(below); }
        )};
        if (not falling.at(above) and support_count == 0) {
          n += 1;
          falling.at(above) = true;
          q.push_back(above);
        }
      }
    }
    return n;
  }));
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const SupportGraph sg{views::istream<Brick>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(sg)};
  const auto part2{find_part2(sg)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
