// clang-format off
import std;
import my_std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

struct Wins {
  long count;
};

std::istream& operator>>(std::istream& is, Wins& w) {
  using aoc::skip;
  using std::operator""s;

  if (unsigned id; skip(is, "Card"s) && is >> id && skip(is, ":"s)) {
    if (std::string tmp; std::getline(is, tmp, '|') && !tmp.empty()) {
      std::stringstream win_str{tmp};
      const auto win{views::istream<int>(win_str) | ranges::to<std::unordered_set>()};

      if (std::getline(is, tmp) && !tmp.empty()) {
        std::stringstream given_str{tmp};
        const auto count{
          ranges::count_if(
            views::istream<int>(given_str),
            [&win](const auto c) { return win.contains(c); }
          )
        };
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

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

auto find_part1(const auto& wins) {
  auto points{
    wins
    | views::filter(std::identity{})
    | views::transform([](const auto w) { return 1 << (w - 1); })
  };
  return sum(points);
}

auto find_part2(const auto& wins) {
  std::vector<long> cards(wins.size(), 1);
  auto card{cards.begin()};
  for (auto win_count : wins) {
    ranges::transform(
      views::repeat(*card, win_count),
      ranges::subrange(++card, cards.end()),
      card,
      std::plus{}
    );
  }
  return sum(cards);
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto wins{
    views::istream<Wins>(std::cin)
      | views::transform([](const auto w) { return w.count; })
      | ranges::to<std::vector>()
  };

  const auto part1{find_part1(wins)};
  const auto part2{find_part2(wins)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
