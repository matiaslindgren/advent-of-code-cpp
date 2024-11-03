#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Claim {
  int id{};
  int left{};
  int top{};
  int width{};
  int height{};

  [[nodiscard]]
  auto iter_yx() const {
    return my_std::views::cartesian_product(views::iota(0, height), views::iota(0, width));
  }
};

auto max_value(const auto& claims, int Claim::* const begin, int Claim::* const len) {
  const auto max_claim{ranges::max(claims, {}, [&](const Claim& c) { return c.*begin + c.*len; })};
  return max_claim.*begin + max_claim.*len;
}

auto count_squares(const auto& claims) {
  const auto width{1 + max_value(claims, &Claim::left, &Claim::width)};
  const auto height{1 + max_value(claims, &Claim::top, &Claim::height)};

  std::vector<int> claim_counts(width * height, 0);

  for (const Claim& c : claims) {
    for (auto&& [y, x] : c.iter_yx()) {
      claim_counts[(y + c.top) * width + x + c.left] += 1;
    }
  }

  const auto overlapping_count{ranges::count_if(claim_counts, [](auto n) { return n > 1; })};

  const auto intact_claim{ranges::find_if(claims, [&](const auto& c) {
    for (auto&& [y, x] : c.iter_yx()) {
      if (claim_counts[(y + c.top) * width + x + c.left] != 1) {
        return false;
      }
    }
    return true;
  })};

  return std::pair{overlapping_count, intact_claim->id};
}

std::istream& operator>>(std::istream& is, Claim& claim) {
  if (int id{}; is >> std::ws >> skip("#"s) >> id) {
    if (int left{}; is >> std::ws >> skip("@"s) >> left) {
      if (int top{}; is >> skip(","s) >> top) {
        if (int width{}; is >> skip(":"s) >> width) {
          if (int height{}; is >> skip("x"s) >> height) {
            claim = {id, left, top, width, height};
          }
        }
      }
    }
  }
  return is;
}

int main() {
  const auto claims{aoc::parse_items<Claim>("/dev/stdin")};
  const auto [part1, part2]{count_squares(claims)};
  std::println("{} {}", part1, part2);
  return 0;
}
