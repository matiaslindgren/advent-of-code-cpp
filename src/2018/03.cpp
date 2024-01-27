import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Claim {
  int id{}, left{}, top{}, width{}, height{};
};

std::istream& operator>>(std::istream& is, Claim& claim) {
  using aoc::skip;
  using std::operator""s;
  if (int id; is >> std::ws && skip(is, "#"s) && is >> id) {
    if (int left; is >> std::ws && skip(is, "@"s) && is >> left) {
      if (int top; skip(is, ","s) && is >> top) {
        if (int width; skip(is, ":"s) && is >> width) {
          if (int height; skip(is, "x"s) && is >> height) {
            claim = {id, left, top, width, height};
          }
        }
      }
    }
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Claim");
}

auto max_value(const auto& claims, int Claim::*const begin, int Claim::*const len) {
  const auto max_claim{*ranges::max_element(claims, ranges::less{}, [&](const Claim& c) {
    return c.*begin + c.*len;
  })};
  return max_claim.*begin + max_claim.*len;
}

auto count_squares(const auto& claims) {
  const auto width{1 + max_value(claims, &Claim::left, &Claim::width)};
  const auto height{1 + max_value(claims, &Claim::top, &Claim::height)};

  std::vector<int> claim_counts(width * height, 0);

  for (const auto& c : claims) {
    for (int y{0}; y < c.height; ++y) {
      for (int x{0}; x < c.width; ++x) {
        claim_counts[(y + c.top) * width + x + c.left] += 1;
      }
    }
  }

  const auto overlapping_count{ranges::count_if(claim_counts, [](auto n) { return n > 1; })};

  const auto intact_claim{ranges::find_if(claims, [&](const auto& c) {
    for (int y{0}; y < c.height; ++y) {
      for (int x{0}; x < c.width; ++x) {
        if (claim_counts[(y + c.top) * width + x + c.left] != 1) {
          return false;
        }
      }
    }
    return true;
  })};

  return std::pair{overlapping_count, intact_claim->id};
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto claims{views::istream<Claim>(input) | ranges::to<std::vector>()};

  const auto [part1, part2]{count_squares(claims)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
