#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Vec2 = ndvec::vec2<int>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

struct Image {
  std::unordered_map<Vec2, int> pixels;
  std::vector<int> algorithm;

  std::size_t extract_index(const Vec2 mid, int default_px) const {
    auto idx{0uz};
    auto i{0uz};
    for (Vec2 p(1, 1); p.y() >= -1; --p.y()) {
      for (p.x() = 1; p.x() >= -1; --p.x()) {
        int px{pixels.contains(mid + p) ? pixels.at(mid + p) : default_px};
        idx += px << i;
        ++i;
      }
    }
    return idx;
  }

  Image enhance_step(const int default_px) const {
    Image img{*this};
    auto [top, bottom]{ranges::minmax(pixels | views::keys)};
    top -= Vec2(1, 1);
    bottom += Vec2(1, 1);
    for (Vec2 p{top}; p.y() <= bottom.y(); ++p.y()) {
      for (p.x() = top.x(); p.x() <= bottom.x(); ++p.x()) {
        img.pixels[p] = algorithm.at(extract_index(p, default_px));
      }
    }
    return img;
  }

  Image enhance(const int n_steps) const {
    Image img{*this};
    for (int step{}; step < n_steps; ++step) {
      int default_px{step % 2 == 0 ? 0 : algorithm.at(0)};
      img = img.enhance_step(default_px);
    }
    return img;
  }

  auto count_light() const {
    return sum(pixels | views::values);
  }
};

int parse_pixel(char ch) {
  if (ch == '#') {
    return 1;
  }
  if (ch == '.') {
    return 0;
  }
  throw std::runtime_error(std::format("unknown pixel '{}'", ch));
}

Image parse_image(std::string path) {
  Image img;
  std::istringstream is{aoc::slurp_file(path)};
  if (std::string line; std::getline(is, line)) {
    img.algorithm = line | views::transform(parse_pixel) | ranges::to<std::vector>();
    for (Vec2 p; std::getline(is >> std::ws, line); p.y() += 1) {
      p.x() = 0;
      for (char ch : line) {
        img.pixels[p] = parse_pixel(ch);
        p.x() += 1;
      }
    }
    if (is.eof()) {
      return img;
    }
  }
  throw std::runtime_error("failed parsing image");
}

int main() {
  const Image img{parse_image("/dev/stdin")};

  const auto part1{img.enhance(2).count_light()};
  const auto part2{img.enhance(50).count_light()};

  std::println("{} {}", part1, part2);

  return 0;
}
