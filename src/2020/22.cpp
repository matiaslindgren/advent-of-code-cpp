#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Cards = std::vector<int>;

auto parse_input(std::string path) {
  const auto sections{
      views::split(aoc::slurp_file(path), "\n\n"s)
      | views::transform([](auto&& s) { return ranges::to<std::string>(s); })
      | ranges::to<std::vector>()
  };
  if (sections.size() == 2) {
    if (std::istringstream is1{sections[0]}; is1 >> skip("Player 1:"s)) {
      auto cards1{views::istream<int>(is1) | ranges::to<Cards>()};
      if (std::istringstream is2{sections[1]}; is2 >> skip("Player 2:"s)) {
        auto cards2{views::istream<int>(is2) | ranges::to<Cards>()};
        if (is1.eof() and is2.eof()) {
          return std::pair{cards1, cards2};
        }
      }
    }
  }
  throw std::runtime_error("input must contain 2 sections separated by 2 newlines");
}

auto pop_front(Cards& cards1, Cards& cards2) {
  int c1{cards1.front()};
  int c2{cards2.front()};
  cards1.erase(cards1.begin());
  cards2.erase(cards2.begin());
  return std::pair{c1, c2};
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0uz, std::plus{})};

auto hash(const Cards& cs) {
  return sum(views::transform(my_std::views::enumerate(cs, 1), my_std::apply_fn(std::multiplies{}))
  );
}

auto take(const Cards& cs, const auto n) {
  return cs | views::take(n) | ranges::to<Cards>();
}

auto score(const Cards& cards) {
  const auto points{views::transform(
      my_std::views::enumerate(cards),
      [n = cards.size()](auto&& ic) {
        auto [i, card]{ic};
        return card * (n - i);
      }
  )};
  return sum(points);
}

auto play(Cards cards1, Cards cards2, bool recursive) {
  for (std::unordered_set<std::size_t> seen; not cards1.empty() and not cards2.empty();) {
    if (auto [_, unseen]{seen.insert(hash(cards1) * hash(cards2))}; not unseen) {
      return std::pair{1uz, 0uz};
    }
    auto [card1, card2]{pop_front(cards1, cards2)};
    bool one_wins{};
    if (not recursive or cards1.size() < card1 or cards2.size() < card2) {
      one_wins = card1 > card2;
    } else {
      const auto [score1, score2]{play(take(cards1, card1), take(cards2, card2), true)};
      one_wins = score1 > score2;
    }
    if (one_wins) {
      cards1.append_range(std::array{card1, card2});
    } else {
      cards2.append_range(std::array{card2, card1});
    }
  }
  return std::pair{score(cards1), score(cards2)};
}

auto find_part1(const Cards& cards1, const Cards& cards2) {
  auto [score1, score2]{play(cards1, cards2, false)};
  return std::max(score1, score2);
}

auto find_part2(const Cards& cards1, const Cards& cards2) {
  auto [score1, score2]{play(cards1, cards2, true)};
  return std::max(score1, score2);
}

int main() {
  const auto [cards1, cards2]{parse_input("/dev/stdin")};

  const auto part1{find_part1(cards1, cards2)};
  const auto part2{find_part2(cards1, cards2)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
