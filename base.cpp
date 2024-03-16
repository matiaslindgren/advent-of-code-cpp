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
  if (std::string line; std::getline(is, line) and not line.empty()) {
    std::stringstream ls{line};
    if (int id; ls >> id) {
      if (std::string str; ls >> str) {
        item = {id, str};
      }
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Item");
}

inline constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};

auto find_part1(const auto& items) {
  return sum(views::transform(items, &Item::id));
}

auto find_part2(const auto& items) {
  return 0;
}

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  auto items{views::istream<Item>(is) | ranges::to<std::vector>()};
  if (is.eof()) {
    return items;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto items{parse_input("/dev/stdin")};

  ranges::copy(items, std::ostream_iterator<Item>(std::cout, "\n"));

  const auto part1{find_part1(items)};
  const auto part2{find_part2(items)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
