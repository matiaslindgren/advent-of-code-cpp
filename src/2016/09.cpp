#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Marker {
  int length;
  int repeat;
  int width;
  int begin = 0;
  constexpr bool is_letter() const {
    return repeat == 0;
  }
};

std::istream& operator>>(std::istream& is, Marker& marker) {
  if (char ch; is >> ch) {
    if (ch == '(') {
      if (std::string m_str; std::getline(is, m_str, ')')) {
        std::istringstream ms{m_str};
        if (int length, repeat; ms >> length and ms.ignore(1, 'x') and ms >> repeat) {
          const int width{static_cast<int>(m_str.size())};
          marker = {length, repeat, width + 2};
          return is;
        }
      }
    } else {
      marker = {0, 0, 1};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Marker");
}

using Markers = std::vector<Marker>;

long count_decompressed(const Markers& markers, const bool simple) {
  std::size_t str_pos{};
  Markers repeating;

  const auto drop_nonrepeating{[&] {
    const auto rm{ranges::remove_if(repeating, [&str_pos](const auto& m) {
      return m.begin + m.length <= str_pos;
    })};
    repeating.erase(rm.begin(), rm.end());
  }};

  const auto get_current_total_repeat{[&] {
    return ranges::fold_left(repeating | views::transform(&Marker::repeat), 1L, std::multiplies{});
  }};

  long n{};
  for (const auto& m : markers) {
    drop_nonrepeating();
    str_pos += m.width;
    if (m.is_letter() or (simple and not repeating.empty())) {
      n += m.width * get_current_total_repeat();
    } else {
      Marker r = m;
      r.begin = str_pos;
      repeating.push_back(r);
    }
  }
  return n;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto markers{views::istream<Marker>(input) | ranges::to<Markers>()};
  const auto part1{count_decompressed(markers, true)};
  const auto part2{count_decompressed(markers, false)};
  std::println("{} {}", part1, part2);

  return 0;
}
