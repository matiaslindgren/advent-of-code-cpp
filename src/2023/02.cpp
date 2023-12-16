import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct GameMax {
  int id;
  int red;
  int green;
  int blue;
};

std::istream& operator>>(std::istream& is, GameMax& gm) {
  using aoc::skip;
  using std::operator""s;
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    if (int id; skip(ls, "Game"s) && ls >> id && skip(ls, ":"s)) {
      std::unordered_map<std::string, int> counts;
      while (ls && is) {
        if (auto [count, cube] = std::pair(int{}, ""s); ls >> count >> cube) {
          if (cube.ends_with(",") || cube.ends_with(";")) {
            cube.pop_back();
          }
          if (cube == "red" || cube == "green" || cube == "blue") {
            counts[cube] = std::max(counts[cube], count);
          } else {
            is.setstate(std::ios_base::failbit);
          }
        }
      }
      if (is) {
        gm = {id, counts["red"], counts["green"], counts["blue"]};
        return is;
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing GameMax");
}

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto max_counts{views::istream<GameMax>(std::cin) | ranges::to<std::vector>()};

  auto possible_ids{
      max_counts
      | views::filter([](auto gm) { return gm.red <= 12 && gm.green <= 13 && gm.blue <= 14; })
      | views::transform([](auto gm) { return gm.id; })
  };
  auto powers{max_counts | views::transform([](auto gm) { return gm.red * gm.green * gm.blue; })};

  const auto part1{sum(possible_ids)};
  const auto part2{sum(powers)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
