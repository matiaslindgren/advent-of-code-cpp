import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

inline constexpr auto width{25};
inline constexpr auto height{6};

auto find_part1(const auto& layers) {
  const auto l_zero{ranges::min(layers, {}, [](auto&& l) { return ranges::count(l, '0'); })};
  return ranges::count(l_zero, '1') * ranges::count(l_zero, '2');
}

auto find_part2(const auto& layers) {
  std::string res;
  for (auto left{0uz}; left < width; left += 5) {
    std::string chunk;
    for (auto y{0uz}; y < height; ++y) {
      for (auto x{0uz}; x < 4; ++x) {
        const auto px_stack{layers | views::transform([&](auto&& l) {
                              return l.at(y * width + left + x);
                            })};
        const auto px{ranges::find_if(px_stack, [](char ch) { return ch != '2'; })};
        chunk.push_back(px != px_stack.end() and *px == '1' ? '#' : '.');
      }
    }
    res.push_back(aoc::ocr(chunk));
  }
  return res;
}

auto parse_layers(const std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (std::string line; std::getline(is, line) >> std::ws and is.eof() and not line.empty()) {
    const auto n{width * height};
    if (line.size() % n) {
      throw std::runtime_error("input must be divisible into layers");
    }
    // TODO views::chunk
    return (
        views::iota(0uz, line.size() - n + 1) | my_std::views::stride(n)
        | views::transform([&](auto&& i) { return line.substr(i, n); }) | ranges::to<std::vector>()
    );
  }
  throw std::runtime_error("input should be one line");
}

int main() {
  const auto layers{parse_layers("/dev/stdin")};

  const auto part1{find_part1(layers)};
  const auto part2{find_part2(layers)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
