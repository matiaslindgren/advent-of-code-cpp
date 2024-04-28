#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec3 = ndvec::vec3<long>;
using aoc::skip;
using std::operator""s;

struct Bot {
  Vec3 p;
  long r{};

  bool in_range(const Bot& rhs) const {
    return p.distance(rhs.p) <= r;
  }
};

std::istream& operator>>(std::istream& is, Bot& bot) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (Bot b; ls >> std::ws >> skip("pos=<"s) >> b.p >> skip(">"s, "r="s) >> b.r) {
      bot = b;
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Bot");
}

auto find_part1(const auto& bots) {
  const auto b1{ranges::max(bots, {}, &Bot::r)};
  return ranges::count_if(bots, [&b1](const auto& b2) { return b1.in_range(b2); });
}

auto find_part2(const auto& bots) {
  // there's a lot of discussion on reddit pointing out the input is poorly constructed
  // and the intended solution, using a greedy 3D octatree scan, is not required.
  // however, the intended solution does NOT solve the general case,
  // which requires a MUCH more complicated approach using 4D coordinates.
  //
  // I copied the solution below so I don't have to think about this anymore
  // https://www.reddit.com/r/adventofcode/comments/a8s17l/comment/ecdqzdg
  // (2024-02-19)
  //
  using P2 = std::pair<long, long>;
  std::priority_queue<P2, std::vector<P2>, std::greater<P2>> q;
  for (const auto& bot : bots) {
    const auto d{bot.p.distance(Vec3{})};
    // begin line
    q.emplace(std::max(0L, d - bot.r), 1);
    // end line
    q.emplace(d + bot.r + 1, -1);
  }
  long count{}, max_count{}, result{};
  for (; not q.empty(); q.pop()) {
    const auto [dist, n]{q.top()};
    count += n;
    if (count > max_count) {
      // max number of overlapping segments
      result = dist;
      max_count = count;
    }
  }
  return result + 1;
}

int main() {
  const auto bots{aoc::slurp<Bot>("/dev/stdin")};

  const auto part1{find_part1(bots)};
  const auto part2{find_part2(bots)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
