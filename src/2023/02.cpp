#include "aoc.hpp"
#include "std.hpp"

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
    std::istringstream ls{line};
    if (int id; ls >> skip("Game"s) >> id >> skip(":"s)) {
      std::unordered_map<std::string, int> counts;
      while (ls and is) {
        if (auto [count, cube] = std::pair(int{}, ""s); ls >> count >> cube) {
          if (cube.ends_with(",") or cube.ends_with(";")) {
            cube.pop_back();
          }
          if (cube == "red" or cube == "green" or cube == "blue") {
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

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  const auto max_counts{aoc::slurp<GameMax>("/dev/stdin")};

  auto possible_ids{
      max_counts
      | views::filter([](auto gm) { return gm.red <= 12 and gm.green <= 13 and gm.blue <= 14; })
      | views::transform(&GameMax::id)
  };
  auto powers{max_counts | views::transform([](auto gm) { return gm.red * gm.green * gm.blue; })};

  const auto part1{sum(possible_ids)};
  const auto part2{sum(powers)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
