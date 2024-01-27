import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Item {
  int id;
  std::string str;
};

std::ostream& operator<<(std::ostream& os, const Item& item) {
  os << "Item {";
  os << ' ' << item.id;
  os << ' ' << item.str;
  os << " }";
  return os;
}

std::istream& operator>>(std::istream& is, Item& item) {
  if (std::string line; std::getline(is, line)) {
    std::stringstream ls{line};
    if (int id; ls >> id) {
      if (std::string str; ls >> str) {
        item = {id, str};
      }
    }
  }
  if (is || is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Item");
}

constexpr auto sum{std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0, std::plus{})};

auto find_part1(const auto& items) {
  return sum(views::transform(items, [](auto i) { return i.id; }));
}

auto find_part2(const auto& items) {
  return 0;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto items{views::istream<Item>(input) | ranges::to<std::vector>()};

  ranges::copy(items, std::ostream_iterator<Item>(std::cout, "\n"));

  const auto part1{find_part1(items)};
  const auto part2{find_part2(items)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
