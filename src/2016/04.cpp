import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Room {
  std::string name;
  std::string checksum;
  int id;

  static constexpr auto alphabet_size{'z' - 'a' + 1};

  std::string check_name() const {
    using CharCounts = std::array<std::pair<char, int>, alphabet_size>;
    CharCounts cc;
    for (char ch : name) {
      if ('a' <= ch && ch <= 'z') {
        auto& p{cc[ch - 'a']};
        p.first = ch;
        p.second += 1;
      }
    }
    ranges::sort(cc, [](const auto& p1, const auto& p2) {
      const auto& [ch1, n1] = p1;
      const auto& [ch2, n2] = p2;
      return n1 > n2 || (n1 == n2 && ch1 < ch2);
    });
    return views::take(cc, 5) | views::elements<0> | ranges::to<std::string>();
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
      | views::filter([](const auto& r) { return r.check_name() == r.checksum; })
      | views::transform(&Room::id),
    0L,
    std::plus{}
  );
  // clang-format on
}

int find_part2(const auto& rooms) {
  using std::operator""sv;
  if (const auto it{ranges::find_if(
          rooms,
          [](const auto& r) {
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
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto rooms{views::istream<Room>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(rooms)};
  const auto part2{find_part2(rooms)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
