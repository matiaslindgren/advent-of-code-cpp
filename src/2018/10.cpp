#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Vec2 = ndvec::vec2<int>;
using aoc::skip;
using std::operator""s;

struct Light {
  Vec2 p;
  Vec2 v;
};

auto find_grid_corners(const auto& lights) {
  return ranges::fold_left(
      lights,
      std::pair{lights.at(0).p, lights.at(0).p},
      [](const auto& corners, const Light& l) {
        auto&& [tl, br]{corners};
        return std::pair{tl.min(l.p), br.max(l.p)};
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
  for (auto i{0UZ}; i < w; i += 8) {
    auto& chunk{chunks.emplace_back()};
    for (auto y{0UZ}; y < h; ++y) {
      for (auto x{0UZ}; x < 6; ++x) {
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

int main() {
  const auto lights{aoc::parse_items<Light>("/dev/stdin", ',')};
  const auto [part1, part2]{wait_for_message(lights)};
  std::println("{} {}", part1, part2);
  return 0;
}
