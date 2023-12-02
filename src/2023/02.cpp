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
  if (int id; skip(is, "Game"s) && is >> id && skip(is, ":"s)) {
    int red{}, green{}, blue{};
    while (is && is.peek() != '\n') {
      if (int count; is >> count) {
        if (std::string cube; is >> cube) {
          if (cube.ends_with(",") || cube.ends_with(";")) {
            cube.pop_back();
          }
          if (cube == "red") {
            red = std::max(red, count);
          } else if (cube == "green") {
            green = std::max(green, count);
          } else if (cube == "blue") {
            blue = std::max(blue, count);
          } else {
            is.setstate(std::ios_base::failbit);
          }
        }
      }
    }
    if (is) {
      is.ignore(1, '\n');
      gm = {id, red, green, blue};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing GameMax");
}

static constexpr auto accumulate{
    std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus<int>())
};

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto cube_max_counts{
      views::istream<GameMax>(std::cin) | ranges::to<std::vector<GameMax>>()
  };

  auto possible_ids{
      cube_max_counts
      | views::filter([](auto gm) { return gm.red <= 12 && gm.green <= 13 && gm.blue <= 14; })
      | views::transform([](auto gm) { return gm.id; })
  };
  auto powers{cube_max_counts | views::transform([](auto gm) {
                return gm.red * gm.green * gm.blue;
              })};

  const auto part1{accumulate(possible_ids)};
  const auto part2{accumulate(powers)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
