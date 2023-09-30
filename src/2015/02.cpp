import std;

// TODO(llvm18)
namespace my_std {
namespace ranges {
// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_left
// (accessed 2023-09-30)
struct fold_left_fn {
  template <std::input_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U =
        std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
    if (first == last) return U(std::move(init));
    U accum = std::invoke(f, std::move(init), *first);
    for (++first; first != last; ++first)
      accum = std::invoke(f, std::move(accum), *first);
    return std::move(accum);
  }

  template <std::ranges::input_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r),
                   std::ranges::end(r),
                   std::move(init),
                   std::ref(f));
  }
};

inline constexpr fold_left_fn fold_left;
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
  throw std::runtime_error("failed parsing present");
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto presents = std::views::istream<Present>(std::cin) |
                        std::ranges::to<std::vector<Present>>();

  constexpr auto accumulate = std::bind(my_std::ranges::fold_left,
                                        std::placeholders::_1,
                                        0L,
                                        std::plus<long>());

  const auto part1 =
      accumulate(std::views::transform(presents, [](const auto& p) {
        return p.surface_area() + p.slack_size();
      }));
  const auto part2 =
      accumulate(std::views::transform(presents, [](const auto& p) {
        return p.ribbon_size() + p.bow_size();
      }));
  std::print("{} {}\n", part1, part2);

  return 0;
}
