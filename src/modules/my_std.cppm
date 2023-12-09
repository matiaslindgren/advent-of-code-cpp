module;
// simplified replacements for stuff currently missing from libc++

import std;

export module my_std;

export namespace my_std {
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

// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_right
// (accessed 2023-12-09)
struct fold_right_fn {
  template <std::bidirectional_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U = std::decay_t<std::invoke_result_t<F&, std::iter_reference_t<I>, T>>;
    if (first == last) return U(std::move(init));
    I tail = std::ranges::next(first, last);
    U accum = std::invoke(f, *--tail, std::move(init));
    while (first != tail) accum = std::invoke(f, *--tail, std::move(accum));
    return accum;
  }

  template <std::ranges::bidirectional_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r), std::ranges::end(r), std::move(init), std::ref(f));
  }
};

inline constexpr fold_right_fn fold_right;

// TODO(llvm18?) P1899R3
template <std::ranges::input_range V>
  requires std::ranges::view<V>
class stride_view : public std::ranges::view_interface<stride_view<V>> {
  using RDiff = std::ranges::range_difference_t<V>;

  V base_;
  RDiff stride_;
  class iterator_;

 public:
  stride_view() = default;

  constexpr explicit stride_view(V base, RDiff stride) : base_{std::move(base)}, stride_{stride} {
  }

  constexpr V base() const&
    requires std::copy_constructible<V>
  {
    return base_;
  }

  constexpr V base() && {
    return std::move(base_);
  }

  constexpr RDiff stride() const noexcept {
    return stride_;
  }

  constexpr auto begin() {
    return iterator_(this, std::ranges::begin(base_));
  }

  constexpr auto end() {
    return iterator_(this, std::ranges::end(base_));
  }
};

template <class R>
stride_view(R&&, std::ranges::range_difference_t<R>) -> stride_view<std::views::all_t<R>>;

template <std::ranges::input_range V>
  requires std::ranges::view<V>
class stride_view<V>::iterator_ {
  using Parent = stride_view;
  using Base = V;

  std::ranges::iterator_t<Base> current_ = std::ranges::iterator_t<Base>();
  std::ranges::sentinel_t<Base> end_ = std::ranges::sentinel_t<Base>();
  std::ranges::range_difference_t<Base> stride_ = 0;
  std::ranges::range_difference_t<Base> missing_ = 0;

 public:
  using difference_type = std::ranges::range_difference_t<Base>;
  using value_type = std::ranges::range_value_t<Base>;

  iterator_()
    requires std::default_initializable<std::ranges::iterator_t<Base>>
  = default;

  constexpr explicit iterator_(Parent* parent, std::ranges::iterator_t<Base> current)
      : current_{std::move(current)},
        end_{std::ranges::end(parent->base_)},
        stride_{parent->stride_} {};

  constexpr const std::ranges::iterator_t<Base>& base() const& noexcept {
    return current_;
  }

  constexpr decltype(auto) operator*() const {
    return *current_;
  }

  constexpr iterator_& operator++() {
    missing_ = std::ranges::advance(current_, stride_, end_);
    return *this;
  }

  constexpr void operator++(int) {
    ++(*this);
  }

  constexpr iterator_ operator++(int)
    requires std::ranges::forward_range<Base>
  {
    auto r = *this;
    ++(*this);
    return r;
  }

  friend constexpr bool operator==(const iterator_& x, std::default_sentinel_t) {
    return x.current_ == x.end_;
  }

  friend constexpr bool operator==(const iterator_& x, const iterator_& y)
    requires std::equality_comparable<std::ranges::iterator_t<Base>>
  {
    return x.current_ == y.current_;
  }

  constexpr iterator_& operator+=(difference_type n)
    requires std::ranges::random_access_range<Base>
  {
    if (n > 0) {
      missing_ = std::ranges::advance(current_, stride_ * n, end_);
    } else if (n < 0) {
      std::ranges::advance(current_, stride_ * n + missing_);
      missing_ = 0;
    }
    return *this;
  }

  constexpr iterator_& operator-=(difference_type n)
    requires std::ranges::random_access_range<Base>
  {
    return *this += -n;
  }

  friend constexpr iterator_ operator+(const iterator_& x, difference_type n)
    requires std::ranges::random_access_range<Base>
  {
    auto r = x;
    r += n;
    return r;
  }

  friend constexpr iterator_ operator-(const iterator_& x, difference_type n)
    requires std::ranges::random_access_range<Base>
  {
    auto r = x;
    r -= n;
    return r;
  }
};

// TODO(llvm18) P2442R1
// chunk

}  // namespace ranges

namespace views {
// TODO(llvm18?) P1899R3
struct _stride_fn : public std::__range_adaptor_closure<_stride_fn> {
  template <class Range, class Step>
  constexpr auto operator()(Range&& r, Step&& step) const {
    return my_std::ranges::stride_view(std::forward<Range>(r), std::forward<Step>(step));
  }

  template <class Step>
  constexpr auto operator()(Step&& step) const {
    return std::__range_adaptor_closure_t(std::__bind_back(*this, std::forward<Step>(step)));
  }
};

inline constexpr auto stride = _stride_fn{};

// TODO P2164R9 properly (or wait for libc++...)
struct _enumerate_fn : public std::__range_adaptor_closure<_enumerate_fn> {
  template <class R>
    requires std::ranges::sized_range<R>
  constexpr decltype(auto) operator()(R&& r) const {
    return std::views::zip(std::views::iota(0uz, r.size()), r);
  }
};

inline constexpr auto enumerate = _enumerate_fn{};

}  // namespace views

}  // namespace my_std
