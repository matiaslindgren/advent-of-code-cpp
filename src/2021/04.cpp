#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

auto int_sqrt(auto a) {
  std::size_t b{1};
  while (b * b < a) {
    b += 1;
  }
  return b;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

class Board {
  std::vector<int> numbers;
  std::vector<bool> marks;
  std::size_t width{};

 public:
  Board(std::vector<int> nums)
      : numbers(nums), marks(nums.size(), false), width(int_sqrt(nums.size())) {
  }

  [[nodiscard]]
  bool is_marked(auto y, auto x) const {
    return marks.at(y * width + x);
  }

  [[nodiscard]]
  bool won() const {
    const auto idx{views::iota(0UZ, width)};
    bool won_row{ranges::any_of(idx, [&](auto y) {
      return ranges::all_of(idx, [&](auto x) { return is_marked(y, x); });
    })};
    bool won_col{ranges::any_of(idx, [&](auto x) {
      return ranges::all_of(idx, [&](auto y) { return is_marked(y, x); });
    })};
    return won_row or won_col;
  }

  [[nodiscard]]
  int sum_of_unmarked() const {
    return sum(views::transform(views::zip(numbers, marks), [](auto&& nm) {
      auto [num, mark]{nm};
      return num * int{not mark};
    }));
  }

  void update_marks(int bingo_num) {
    for (auto&& [num, mark] : views::zip(numbers, marks)) {
      if (num == bingo_num) {
        mark = true;
      }
    }
  }
};

auto play(const auto& numbers, auto boards) {
  std::vector<int> scores;
  for (int num : numbers) {
    for (Board& b : boards) {
      b.update_marks(num);
      if (b.won()) {
        scores.push_back(num * b.sum_of_unmarked());
      }
    }
    boards = views::filter(boards, [](const Board& b) { return not b.won(); })
             | ranges::to<std::vector>();
  }
  if (scores.empty()) {
    throw std::runtime_error("must have at least 1 score after playing");
  }
  return std::pair{scores.front(), scores.back()};
}

auto parse_numbers(const std::string& s) {
  std::istringstream is{s};
  return views::istream<int>(is) | ranges::to<std::vector>();
}

auto parse_input(std::string path) {
  auto sections{
      aoc::slurp_file(path) | views::split("\n\n"s)
      | views::transform([](auto&& s) { return ranges::to<std::string>(s); })
      | ranges::to<std::vector>()
  };
  if (sections.size() < 2) {
    throw std::runtime_error("input must contain at least 2 sections separated by 2 newlines");
  }

  ranges::replace(sections[0], ',', ' ');
  auto numbers{parse_numbers(sections[0])};

  auto boards{
      sections | views::drop(1) | views::transform([](auto&& s) { return Board(parse_numbers(s)); })
      | ranges::to<std::vector>()
  };

  return std::pair{numbers, boards};
}

int main() {
  const auto [numbers, boards]{parse_input("/dev/stdin")};
  const auto [part1, part2]{play(numbers, boards)};
  std::println("{} {}", part1, part2);
  return 0;
}
