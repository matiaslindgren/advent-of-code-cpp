import std;

// TODO(llvm17)
namespace my_ranges {
template <class Container>
  requires(!std::ranges::view<Container>)
constexpr auto to() {
  return std::__range_adaptor_closure_t([]<std::ranges::input_range R>(R &&r) {
    Container c;
    std::ranges::copy(r, std::back_inserter(c));
    return c;
  });
}
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
}  // namespace my_ranges

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
  {
    char ch;
    int l, w, h;
    if (is >> l >> ch && ch == 'x' && is >> w >> ch && ch == 'x' && is >> h) {
      p = {l, w, h};
      return is;
    }
  }
  p = {0};
  is.setstate(std::ios_base::failbit);
  return is;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  const auto presents = std::views::istream<Present>(std::cin) |
                        my_ranges::to<std::vector<Present>>();
  const auto part1 = presents | std::views::transform([](const auto &p) {
                       return p.surface_area() + p.slack_size();
                     }) |
                     my_ranges::fold(0L, std::plus<long>());
  const auto part2 = presents | std::views::transform([](const auto &p) {
                       return p.ribbon_size() + p.bow_size();
                     }) |
                     my_ranges::fold(0L, std::plus<long>());
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
