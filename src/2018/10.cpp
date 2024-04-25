#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = aoc::Vec2<int>;

struct Light {
  Vec2 p, v;
};

using aoc::skip;
using std::operator""s;

auto find_grid_corners(const auto& lights) {
  constexpr auto intmin{std::numeric_limits<int>::min()};
  constexpr auto intmax{std::numeric_limits<int>::max()};
  return ranges::fold_left(
      lights,
      std::pair{Vec2(intmax, intmax), Vec2(intmin, intmin)},
      [](const auto& corners, const auto& l) {
        auto [tl, br] = corners;
        return std::pair{
            Vec2(std::min(tl.x(), l.p.x()), std::min(tl.y(), l.p.y())),
            Vec2(std::max(br.x(), l.p.x()), std::max(br.y(), l.p.y()))
        };
      }
  );
}

auto extract_chunks(const auto& lights, const auto tl, const auto br) {
  const auto h{br.y() - tl.y() + 1};
  const auto w{br.x() - tl.x() + 1};
  std::string sky(h * w, '.');
  for (const auto& l : lights) {
    sky[(l.p.y() - tl.y()) * w + (l.p.x() - tl.x())] = '#';
  }

  std::vector<std::string> chunks;
  for (auto i{0uz}; i < w; i += 8) {
    auto& chunk{chunks.emplace_back()};
    for (auto y{0uz}; y < h; ++y) {
      for (auto x{0uz}; x < 6; ++x) {
        chunk.push_back(sky[y * w + x + i]);
      }
    }
  }
  return chunks;
}

auto wait_for_message(auto lights) {
  for (int t{}; t < 1'000'000; ++t) {
    const auto [tl, br]{find_grid_corners(lights)};
    if (br.y() - tl.y() + 1 == 10 and br.x() - tl.x() < 200) {
      const auto msg{
          extract_chunks(lights, tl, br) | views::transform(aoc::ocr) | ranges::to<std::string>()
      };
      if (ranges::all_of(msg, [](auto ch) { return ch != ' '; })) {
        return std::pair{msg, t};
      }
    }
    ranges::for_each(lights, [](auto& l) { l.p += l.v; });
  }
  throw std::runtime_error("search space exhausted");
}

std::istream& operator>>(std::istream& is, Light& light) {
  if (Vec2 p, v; is >> std::ws >> skip("position=<"s) >> p >> skip(">"s) >> std::ws
                 >> skip("velocity=<"s) >> v >> skip(">"s)) {
    light = {p, v};
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Light");
}

auto parse_input(std::string_view path) {
  auto input{aoc::slurp_file(path)};
  ranges::replace(input, ',', ' ');
  std::istringstream is{input};
  auto lights{std::views::istream<Light>(is) | std::ranges::to<std::vector>()};
  if (lights.empty()) {
    throw std::runtime_error("input is empty");
  }
  if (not is and not is.eof()) {
    throw std::runtime_error("input is invalid");
  }
  return lights;
}

int main() {
  const auto lights{parse_input("/dev/stdin")};
  const auto [part1, part2]{wait_for_message(lights)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
