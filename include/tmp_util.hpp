import std;

// TODO(llvm18?)
namespace my_std {
namespace ranges {
// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_left
// (accessed 2023-09-30)
struct fold_left_fn {
  template <std::input_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U = std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
    if (first == last) return U(std::move(init));
    U accum = std::invoke(f, std::move(init), *first);
    for (++first; first != last; ++first) accum = std::invoke(f, std::move(accum), *first);
    return std::move(accum);
  }

  template <std::ranges::input_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r), std::ranges::end(r), std::move(init), std::ref(f));
  }
};

inline constexpr fold_left_fn fold_left;

// TODO(llvm18?)
template <class Container>
  requires(!std::ranges::view<Container>)
constexpr auto stride(std::size_t n) {
  return std::__range_adaptor_closure_t([=]<std::ranges::input_range R>(R&& r) {
    // awful hack implementation
    Container c;
    std::size_t i{0};
    for (const auto& x : r) {
      if (i++ % n == 0) {
        c.push_back(x);
      }
    }
    return c;
  });
}

}  // namespace ranges
}  // namespace my_std
