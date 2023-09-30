import std;

// TODO(llvm18)
namespace my_std {
namespace ranges {
template <class T, class F>
constexpr auto fold(T init, F f) {
  return std::__range_adaptor_closure_t([=]<std::ranges::input_range R>(R &&r) {
    auto result = init;
    for (const auto &value : r) {
      result = f(result, value);
    }
    return result;
  });
}
}  // namespace ranges
}  // namespace my_std

struct Present {
  int l;
  int w;
  int h;
  long surface_area() const { return 2 * (l * w + w * h + h * l); }
  long slack_size() const { return std::min(std::min(l * w, w * h), h * l); }
  long bow_size() const { return 2 * std::min(std::min(l + w, w + h), h + l); }
  long ribbon_size() const { return l * w * h; }
};

std::istream &operator>>(std::istream &is, Present &p) {
  char ch;
  int l, w, h;
  if (is >> l >> ch && ch == 'x' && is >> w >> ch && ch == 'x' && is >> h) {
    p = {l, w, h};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing present");
}

int main() {
  std::ios_base::sync_with_stdio(false);
  const auto presents = std::views::istream<Present>(std::cin) |
                        std::ranges::to<std::vector<Present>>();
  const auto part1 = presents | std::views::transform([](const auto &p) {
                       return p.surface_area() + p.slack_size();
                     }) |
                     my_std::ranges::fold(0L, std::plus<long>());
  const auto part2 = presents | std::views::transform([](const auto &p) {
                       return p.ribbon_size() + p.bow_size();
                     }) |
                     my_std::ranges::fold(0L, std::plus<long>());
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
