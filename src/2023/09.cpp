import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

using Ints = std::vector<int>;

std::vector<Ints> parse_input(std::istream& is) {
  std::vector<Ints> h;
  for (std::string line; std::getline(is, line);) {
    std::stringstream ls{line};
    h.push_back(views::istream<int>(ls) | ranges::to<Ints>());
  }
  if (is.eof()) {
    return h;
  }
  throw std::runtime_error("failed parsing Ints");
}

// TODO ranges::adjacent
constexpr decltype(auto) window2(auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

Ints adjacent_diff(const auto& hist) {
  return window2(hist) | views::transform([](auto&& w) {
           const auto [x1, x2] = w;
           return x2 - x1;
         })
         | ranges::to<Ints>();
}

constexpr auto sum{
    std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0L, std::plus<long>())
};

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto histories{parse_input(std::cin)};

  long part1{0}, part2{0};
  for (const auto& history : histories) {
    Ints front{}, back{};
    for (Ints diff{history}; ranges::any_of(diff, std::identity{}); diff = adjacent_diff(diff)) {
      front.push_back(diff.front());
      back.push_back(diff.back());
    }
    part1 += sum(back);
    part2 += my_std::ranges::fold_left(front | views::reverse, 0L, [](auto res, auto x) {
      return x - res;
    });
  }

  std::print("{} {}\n", part1, part2);

  return 0;
}
