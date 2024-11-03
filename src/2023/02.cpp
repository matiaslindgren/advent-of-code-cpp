#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct GameMax {
  int id;
  int red;
  int green;
  int blue;
};

std::istream& operator>>(std::istream& is, GameMax& gm) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::istringstream ls{line};
    if (int id{}; ls >> skip("Game"s) >> id >> skip(":"s)) {
      std::unordered_map<std::string, int> counts;
      for (auto [count, cube]{std::pair(int{}, ""s)}; ls >> count >> cube;) {
        if (cube.ends_with(",") or cube.ends_with(";")) {
          cube.pop_back();
        }
        if (cube == "red" or cube == "green" or cube == "blue") {
          counts[cube] = std::max(counts[cube], count);
        } else {
          throw std::runtime_error(std::format("unknown cube '{}'", cube));
        }
      }
      if (not ls.eof()) {
        throw std::runtime_error(std::format("failed parsing line '{}'", line));
      }
      gm = {id, counts["red"], counts["green"], counts["blue"]};
    }
  }
  return is;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

int main() {
  const auto max_counts{aoc::parse_items<GameMax>("/dev/stdin")};

  auto possible_ids{
      max_counts
      | views::filter([](auto gm) { return gm.red <= 12 and gm.green <= 13 and gm.blue <= 14; })
      | views::transform(&GameMax::id)
  };
  auto powers{max_counts | views::transform([](auto gm) { return gm.red * gm.green * gm.blue; })};

  const auto part1{sum(possible_ids)};
  const auto part2{sum(powers)};

  std::println("{} {}", part1, part2);

  return 0;
}
