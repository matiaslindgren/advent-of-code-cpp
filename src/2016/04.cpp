import std;
#include "tmp_util.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Room {
  std::string name;
  std::string checksum;
  int id;

  static constexpr auto alphabet_size = 'z' - 'a' + 1;

  std::string decode_name() const {
    using CharCounts = std::array<std::pair<int, char>, alphabet_size>;
    CharCounts cc;
    for (char ch : name) {
      const auto i{ch - 'a'};
      cc[i].first += 1;
      cc[i].second = ch;
    }
    std::ranges::sort(cc, [](auto&& p1, auto&& p2) {
      auto&& [n1, ch1] = p1;
      auto&& [n2, ch2] = p2;
      return n1 > n2 || (n1 == n2 && ch1 < ch2);
    });
    // clang-format off
    return (
      views::take(cc, 5)
      | views::transform([](auto&& p) { return p.second; })
      | ranges::to<std::string>()
    );
    // clang-format on
  }

  std::string decrypt_name() const {
    return (
        views::transform(
            name,
            [=, this](const char& ch) -> char {
              if ('a' <= ch && ch <= 'z') {
                return 'a' + ((ch - 'a') + id) % alphabet_size;
              }
              return ' ';
            }
        )
        | ranges::to<std::string>()
    );
  }
};

bool isdigit(unsigned char ch) {
  return std::isdigit(ch);
}

std::istream& operator>>(std::istream& is, Room& room) {
  {
    Room r;
    for (char ch; !isdigit(is.peek()) && is.get(ch);) {
      r.name.push_back(ch);
    }
    if (is >> r.id) {
      if (is.ignore(1, '[') && std::getline(is, r.checksum, ']')) {
        room = r;
        return is.ignore(1);
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Room");
}

int find_part1(const auto& rooms) {
  // clang-format off
  return my_std::ranges::fold_left(
    rooms
      | views::filter([](auto&& r) { return r.decode_name() == r.checksum; })
      | views::transform([](auto&& r) { return r.id; }),
    0L,
    std::plus<int>()
  );
  // clang-format on
}

int find_part2(const auto& rooms) {
  using std::operator""sv;
  if (const auto it{ranges::find_if(
          rooms,
          [](auto&& r) {
            const auto& s{r.decrypt_name()};
            return s.starts_with("northpole object storage"sv);
          }
      )};
      it != rooms.end()) {
    return it->id;
  }
  throw std::runtime_error("cannot find northpole object storage");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto rooms{views::istream<Room>(std::cin) | ranges::to<std::vector<Room>>()};

  const auto part1{find_part1(rooms)};
  const auto part2{find_part2(rooms)};
  std::print("{} {}\n", part1, part2);

  return 0;
}
