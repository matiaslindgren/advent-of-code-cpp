import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Vec3 {
  long x{}, y{}, z{};

  Vec3 operator-(const Vec3& v) const {
    return {x - v.x, y - v.y, z - v.z};
  }
  Vec3 operator+(const Vec3& v) const {
    return {x + v.x, y + v.y, z + v.z};
  }
  Vec3 scale(long p, long q) const {
    q = std::max(p, q);
    return {x * p / q, y * p / q, z * p / q};
  }
  Vec3 abs() const {
    return {std::abs(x), std::abs(y), std::abs(z)};
  }
  auto max() const {
    return std::max(std::max(x, y), z);
  }
  auto sum() const {
    return x + y + z;
  }
  auto distance(const Vec3& v) const {
    return (*this - v).abs().sum();
  }
};

std::istream& operator>>(std::istream& is, Vec3& vec) {
  if (Vec3 v; is >> v.x >> skip(","s) >> v.y >> skip(","s) >> v.z) {
    vec = v;
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Vec3");
}

struct Bot {
  Vec3 p;
  long r{};

  bool in_range(const Bot& rhs) const {
    return p.distance(rhs.p) <= r;
  }
};

std::istream& operator>>(std::istream& is, Bot& bot) {
  if (Bot b; is >> std::ws >> skip("pos=<"s) >> b.p >> skip(">, r="s) >> b.r) {
    bot = b;
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Bot");
}

auto find_part1(const auto& bots) {
  const auto b1{*ranges::max_element(bots, {}, &Bot::r)};
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
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto bots{views::istream<Bot>(input) | ranges::to<std::vector>()};
  if (bots.empty()) {
    throw std::runtime_error("input must contain at least one nanobot");
  }

  const auto part1{find_part1(bots)};
  const auto part2{find_part2(bots)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
