#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Marker {
  int length{};
  int repeat{};
  int width{};
  int begin{};

  [[nodiscard]]
  bool is_letter() const {
    return repeat == 0;
  }
};

using Markers = std::vector<Marker>;

enum struct Method : unsigned char {
  simple,
  repeating,
};

constexpr auto product{std::bind_back(ranges::fold_left, 1L, std::multiplies{})};

long count_decompressed(const Markers& markers, const Method method) {
  int str_pos{};
  Markers repeating;

  const auto drop_nonrepeating{[&] {
    std::erase_if(repeating, [&str_pos](const Marker& m) { return m.begin + m.length <= str_pos; });
  }};

  const auto get_current_total_repeat{[&] {
    return product(views::transform(repeating, &Marker::repeat));
  }};

  long n{};
  for (const Marker& m : markers) {
    drop_nonrepeating();
    str_pos += m.width;
    if (m.is_letter() or (method == Method::simple and not repeating.empty())) {
      n += m.width * get_current_total_repeat();
    } else {
      Marker r{m};
      r.begin = str_pos;
      repeating.push_back(r);
    }
  }
  return n;
}

std::istream& operator>>(std::istream& is, Marker& marker) {
  bool ok{false};
  if (char ch{}; is >> ch) {
    if (ch == '(') {
      if (std::string m_str; std::getline(is, m_str, ')')) {
        std::istringstream ms{m_str};
        if (int length{}, repeat{}; ms >> length >> skip("x"s) >> repeat) {
          marker = {length, repeat, static_cast<int>(m_str.size()) + 2};
          ok = true;
        }
      }
    } else {
      marker = {0, 0, 1};
      ok = true;
    }
  }
  if (not is.eof() and not ok) {
    throw std::runtime_error("failed parsing Marker");
  }
  return is;
}

int main() {
  const auto markers{aoc::parse_items<Marker>("/dev/stdin")};

  const auto part1{count_decompressed(markers, Method::simple)};
  const auto part2{count_decompressed(markers, Method::repeating)};

  std::println("{} {}", part1, part2);

  return 0;
}
