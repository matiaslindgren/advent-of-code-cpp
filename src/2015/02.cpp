import std;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Present {
  int l;
  int w;
  int h;
  long surface_area() const {
    return 2 * (l * w + w * h + h * l);
  }
  long slack_size() const {
    return std::min(std::min(l * w, w * h), h * l);
  }
  long bow_size() const {
    return 2 * std::min(std::min(l + w, w + h), h + l);
  }
  long ribbon_size() const {
    return l * w * h;
  }
};

std::istream& operator>>(std::istream& is, Present& p) {
  char ch;
  int l, w, h;
  if (is >> l >> ch && ch == 'x' && is >> w >> ch && ch == 'x' && is >> h) {
    p = {l, w, h};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Present");
}

static constexpr auto accumulate{
    std::bind(my_std::ranges::fold_left, std::placeholders::_1, 0L, std::plus<long>())
};

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto presents = views::istream<Present>(std::cin) | ranges::to<std::vector<Present>>();

  const auto part1{accumulate(views::transform(presents, [](const auto& p) {
    return p.surface_area() + p.slack_size();
  }))};
  const auto part2{accumulate(views::transform(presents, [](const auto& p) {
    return p.ribbon_size() + p.bow_size();
  }))};
  std::print("{} {}\n", part1, part2);

  return 0;
}
