#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Wins {
  long count{};
};

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& wins) {
  auto points{wins | views::filter([](long wc) { return wc > 0; }) | views::transform([](long wc) {
                return 1 << (wc - 1);
              })};
  return sum(points);
}

auto find_part2(const auto& wins) {
  std::vector<long> cards(wins.size(), 1);
  auto card{cards.begin()};
  for (long win_count : wins) {
    ranges::transform(
        views::repeat(*card, win_count),
        ranges::subrange(++card, cards.end()),
        card,
        std::plus{}
    );
  }
  return sum(cards);
}

std::istream& operator>>(std::istream& is, Wins& w) {
  if (unsigned id{}; is >> skip("Card"s) >> id >> skip(":"s)) {
    if (std::string tmp; std::getline(is, tmp, '|') and not tmp.empty()) {
      std::istringstream win_str{tmp};
      const auto win{views::istream<int>(win_str) | ranges::to<std::unordered_set>()};

      if (std::getline(is, tmp) and not tmp.empty()) {
        std::istringstream given_str{tmp};
        const auto count{ranges::count_if(views::istream<int>(given_str), [&win](const auto c) {
          return win.contains(c);
        })};
        w = {count};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Wins");
}

int main() {
  const auto wins{
      aoc::parse_items<Wins>("/dev/stdin") | views::transform(&Wins::count)
      | ranges::to<std::vector>()
  };

  const auto part1{find_part1(wins)};
  const auto part2{find_part2(wins)};

  std::println("{} {}", part1, part2);

  return 0;
}
