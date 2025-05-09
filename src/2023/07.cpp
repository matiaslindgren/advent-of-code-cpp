#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

constexpr int joker_id{1};

int char2card(char ch) {
  switch (ch) {
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return ch - '0';
    case 'T':
      return 10;
    case 'J':
      return 11;
    case 'Q':
      return 12;
    case 'K':
      return 13;
    case 'A':
      return 14;
  }
  throw std::runtime_error(std::format("unknown card face '{}'", ch));
}

struct Hand {
  using Counts = std::array<std::pair<int, int>, 15>;
  using Values = std::array<int, 5>;
  Values values{};

  [[nodiscard]]
  auto count() const {
    Counts counts{};
    for (auto v : values) {
      auto& p = counts.at(v);
      p = {p.first + 1, v};
    }
    return counts;
  }

  bool operator<(const Hand& other) const {
    return ranges::lexicographical_compare(values, other.values);
  }
};

struct Card {
  Hand hand{};
  long bid{};

  [[nodiscard]]
  int rank() const {
    auto counts{hand.count()};

    if (const auto jok{ranges::find_if(counts, [=](auto p) { return p.second == joker_id; })};
        jok != counts.end()) {
      const auto joker_count{std::exchange(jok->first, 0)};
      ranges::max_element(counts)->first += joker_count;
    }

    ranges::sort(counts, ranges::greater{});

    const auto n0{counts[0].first};
    const auto n1{counts[1].first};

    if (n0 == 5) {
      return 7;
    }
    if (n0 == 4) {
      return 6;
    }
    if (n0 == 3 and n1 == 2) {
      return 5;
    }
    if (n0 == 3 and n1 == 1) {
      return 4;
    }
    if (n0 == 2 and n1 == 2) {
      return 3;
    }
    if (n0 == 2 and n1 == 1) {
      return 2;
    }
    return 1;
  }

  bool operator<(const Card& other) const {
    const auto r1{rank()};
    const auto r2{other.rank()};
    return r1 < r2 or (r1 == r2 and hand < other.hand);
  }
};

constexpr auto sum{std::bind_back(ranges::fold_left, 0L, std::plus{})};

using Cards = std::vector<Card>;

auto total_winnings(ranges::random_access_range auto&& r) {
  Cards cards{r | ranges::to<Cards>()};
  std::sort(cards.begin(), cards.end());
  const auto bids{views::transform(cards, &Card::bid)};
  return sum(
      my_std::views::enumerate(bids, 1) | views::transform(my_std::apply_fn(std::multiplies{}))
  );
}

std::istream& operator>>(std::istream& is, Card& card) {
  if (std::string line; std::getline(is, line)) {
    std::istringstream ls{line};
    if (std::string str_hand; ls >> str_hand and str_hand.size() == 5) {
      Hand hand;
      ranges::transform(str_hand, hand.values.begin(), char2card);
      if (int bid{}; ls >> bid) {
        card = {hand, bid};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Card");
}

int main() {
  const auto cards{aoc::parse_items<Card>("/dev/stdin")};

  const auto part1{total_winnings(cards)};
  const auto part2{total_winnings(views::transform(cards, [](auto c) -> Card {
    ranges::replace(c.hand.values, char2card('J'), joker_id);
    return c;
  }))};

  std::println("{} {}", part1, part2);

  return 0;
}
