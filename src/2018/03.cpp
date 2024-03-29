import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Claim {
  int id{}, left{}, top{}, width{}, height{};

  auto iter_yx() const {
    return my_std::views::cartesian_product(views::iota(0, height), views::iota(0, width));
  }
};

std::istream& operator>>(std::istream& is, Claim& claim) {
  using aoc::skip;
  using std::operator""s;
  if (int id; is >> std::ws >> skip("#"s) >> id) {
    if (int left; is >> std::ws >> skip("@"s) >> left) {
      if (int top; is >> skip(","s) >> top) {
        if (int width; is >> skip(":"s) >> width) {
          if (int height; is >> skip("x"s) >> height) {
            claim = {id, left, top, width, height};
          }
        }
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Claim");
}

auto max_value(const auto& claims, int Claim::*const begin, int Claim::*const len) {
  const auto max_claim{ranges::max(claims, {}, [&](const Claim& c) { return c.*begin + c.*len; })};
  return max_claim.*begin + max_claim.*len;
}

auto count_squares(const auto& claims) {
  const auto width{1 + max_value(claims, &Claim::left, &Claim::width)};
  const auto height{1 + max_value(claims, &Claim::top, &Claim::height)};

  std::vector<int> claim_counts(width * height, 0);

  for (const auto& c : claims) {
    for (const auto& [y, x] : c.iter_yx()) {
      claim_counts[(y + c.top) * width + x + c.left] += 1;
    }
  }

  const auto overlapping_count{ranges::count_if(claim_counts, [](auto n) { return n > 1; })};

  const auto intact_claim{ranges::find_if(claims, [&](const auto& c) {
    for (const auto& [y, x] : c.iter_yx()) {
      if (claim_counts[(y + c.top) * width + x + c.left] != 1) {
        return false;
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
