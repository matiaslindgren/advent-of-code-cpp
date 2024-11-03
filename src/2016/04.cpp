#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::is_digit;
using aoc::skip;

struct Room {
  std::string name;
  std::string checksum;
  int id{};

  static constexpr auto alphabet_size{'z' - 'a' + 1};

  [[nodiscard]]
  std::string check_name() const {
    using CharCounts = std::array<std::pair<char, int>, alphabet_size>;
    CharCounts cc;
    for (char ch : name) {
      if ('a' <= ch and ch <= 'z') {
        auto& p{cc.at(ch - 'a')};
        p.first = ch;
        p.second += 1;
      }
    }
    ranges::sort(cc, [](const auto& p1, const auto& p2) {
      auto [ch1, n1]{p1};
      auto [ch2, n2]{p2};
      return n1 > n2 or (n1 == n2 and ch1 < ch2);
    });
    return views::take(cc, 5) | views::elements<0> | ranges::to<std::string>();
  }

  [[nodiscard]]
  std::string decrypt_name() const {
    return views::transform(
               name,
               [this](char ch) -> char {
                 if ('a' <= ch and ch <= 'z') {
                   return static_cast<char>('a' + ((ch - 'a') + id) % alphabet_size);
                 }
                 return ' ';
               }
           )
           | ranges::to<std::string>();
  }
};

constexpr auto sum{std::bind_back(ranges::fold_left, 0L, std::plus{})};

int find_part1(const auto& rooms) {
  return sum(
      views::filter(rooms, [](const Room& r) { return r.check_name() == r.checksum; })
      | views::transform(&Room::id)
  );
}

int find_part2(const auto& rooms) {
  if (const auto it{ranges::find_if(
          rooms,
          [](const Room& r) {
            const auto& s{r.decrypt_name()};
            return s.starts_with("northpole object storage"s);
          }
      )};
      it != rooms.end()) {
    return it->id;
  }
  throw std::runtime_error("cannot find northpole object storage");
}

std::istream& operator>>(std::istream& is, Room& room) {
  Room r;
  for (char ch{}; is >> std::ws and not is_digit(is.peek()) and is.get(ch);) {
    r.name.push_back(ch);
  }
  if (is >> r.id >> skip("["s) and std::getline(is, r.checksum, ']')) {
    room = r;
  }
  return is;
}

int main() {
  const auto rooms{aoc::parse_items<Room>("/dev/stdin")};

  const auto part1{find_part1(rooms)};
  const auto part2{find_part2(rooms)};

  std::println("{} {}", part1, part2);

  return 0;
}
