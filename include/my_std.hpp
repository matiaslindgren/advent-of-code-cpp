#ifndef MY_STD_HEADER_INCLUDED
#define MY_STD_HEADER_INCLUDED
// oversimplified C++23 stuff until libc++ implements these

#include "std.hpp"

namespace my_std {
namespace ranges {
// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_right
// (accessed 2023-12-09)
struct fold_right_fn {
  template <std::bidirectional_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U = std::decay_t<std::invoke_result_t<F&, std::iter_reference_t<I>, T>>;
    if (first == last) {
      return U(std::move(init));
    }
    I tail = std::ranges::next(first, last);
    U accum = std::invoke(f, *--tail, std::move(init));
    while (first != tail) {
      accum = std::invoke(f, *--tail, std::move(accum));
    }
    return accum;
  }

  template <std::ranges::bidirectional_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r), std::ranges::end(r), std::move(init), std::ref(f));
  }
};

inline constexpr fold_right_fn fold_right;

// TODO(llvm19?) P1899R3
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

// TODO P2374R4 (llvm19?)
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2374r4.html

// TODO
template <class... Ts>
// if sizeof...(Ts) == 2
//   using cartesian_product_tuple_or_pair = std::pair<Ts...>;
// else
//   using cartesian_product_tuple_or_pair = std::tuple<Ts...>;
using cartesian_product_tuple_or_pair = std::tuple<Ts...>;

template <class F, class Tuple>
constexpr auto cartesian_product_tuple_transform(F&& f, Tuple&& tuple) {
  return std::apply(
      [&]<class... Ts>(Ts&&... elements) {
        return cartesian_product_tuple_or_pair<std::invoke_result_t<F&, Ts>...>(
            std::invoke(f, std::forward<Ts>(elements))...
        );
      },
      std::forward<Tuple>(tuple)
  );
}

template <std::ranges::input_range First, std::ranges::forward_range... Vs>
  requires(std::ranges::view<First> && ... && std::ranges::view<Vs>)
class cartesian_product_view
    : public std::ranges::view_interface<cartesian_product_view<First, Vs...>> {
 private:
  std::tuple<First, Vs...> bases_;
  class iterator;

 public:
  constexpr cartesian_product_view() = default;

  constexpr explicit cartesian_product_view(First first_base, Vs... bases)
      : bases_(std::move(first_base), std::move(bases)...) {
  }

  constexpr iterator begin() {
    return iterator(this, cartesian_product_tuple_transform(std::ranges::begin, bases_));
  }

  constexpr iterator end() {
    return iterator(
        this,
        std::apply(
            [](auto&& base0, auto&&... bases) {
              return std::tuple{
                  std::ranges::end(base0),
                  std::ranges::begin(bases)...,
              };
            },
            bases_
        )
    );
  }

  constexpr std::default_sentinel_t end() const noexcept {
    return {};
  }
};

template <class... Vs>
cartesian_product_view(Vs&&...) -> cartesian_product_view<std::views::all_t<Vs>...>;

template <std::ranges::input_range First, std::ranges::forward_range... Vs>
  requires(std::ranges::view<First> && ... && std::ranges::view<Vs>)
class cartesian_product_view<First, Vs...>::iterator {
 public:
  using value_type = cartesian_product_tuple_or_pair<
      std::ranges::range_value_t<First>,
      std::ranges::range_value_t<Vs>...>;
  using difference_type = std::ranges::range_difference_t<First>;

 private:
  cartesian_product_view* parent_ = nullptr;
  cartesian_product_tuple_or_pair<std::ranges::iterator_t<First>, std::ranges::iterator_t<Vs>...>
      current_;

  template <std::size_t N = sizeof...(Vs)>
  constexpr void next() {
    auto& it = std::get<N>(current_);
    ++it;
    if constexpr (N) {
      if (it == std::ranges::end(get<N>(parent_->bases_))) {
        it = std::ranges::begin(std::get<N>(parent_->bases_));
        next<N - 1>();
      }
    }
  }

  template <std::size_t N = sizeof...(Vs)>
  constexpr bool equal(const iterator& lhs, const iterator& rhs) const {
    if (std::get<N>(lhs.current_) != std::get<N>(rhs.current_)) {
      return false;
    }
    if constexpr (N) {
      return equal<N - 1>(lhs, rhs);
    }
    return true;
  }

  constexpr bool is_end() const {
    auto&& first_base{std::get<0>(parent_->bases)};
    return std::ranges::end(first_base) == std::get<0>(current_);
  }

 public:
  iterator()
    requires std::ranges::forward_range<First>
  = default;

  constexpr explicit iterator(
      cartesian_product_view* parent,
      cartesian_product_tuple_or_pair<
          std::ranges::iterator_t<First>,
          std::ranges::iterator_t<Vs>...> current
  )
      : parent_(parent), current_(std::move(current)) {
  }

  // constexpr iterator(iterator i) : current_(std::move(i.current_)) { }

  constexpr auto operator*() const {
    return cartesian_product_tuple_transform(
        [](auto& i) -> decltype(auto) { return *i; },
        current_
    );
  }

  constexpr iterator& operator++() {
    next();
    return *this;
  }

  constexpr void operator++(int) {
    ++(*this);
  }

  constexpr bool operator==(const iterator& x) const {
    return equal(*this, x);
  }

  friend constexpr bool operator==(const iterator& x, std::default_sentinel_t) {
    return x.is_end();
  }
};

// TODO(llvm19) P2442R1
// chunk

}  // namespace ranges

namespace views {
// TODO(llvm19?) P1899R3
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
  template <std::ranges::viewable_range R>
  constexpr decltype(auto) operator()(R&& r, std::ranges::range_difference_t<R> start) const {
    if constexpr (std::ranges::sized_range<R>) {
      return std::views::zip(std::views::iota(start, start + std::ranges::distance(r)), r);
    } else {
      return std::views::zip(std::views::iota(start), r);
    }
  }

  template <std::ranges::viewable_range R>
  constexpr decltype(auto) operator()(R&& r) const {
    return this->operator()(std::forward<R>(r), {});
  }
};

inline constexpr auto enumerate = _enumerate_fn{};

struct _cartesian_product_fn {
  template <std::ranges::range... Rs>
  constexpr auto operator()(Rs&&... rs) const {
    return my_std::ranges::cartesian_product_view(std::forward<Rs>(rs)...);
  }
};

inline constexpr auto cartesian_product = _cartesian_product_fn{};

}  // namespace views

struct _apply_fn {
  template <class Fn>
  constexpr auto operator()(Fn&& f) const {
    return [&f]<class Tuple>(Tuple&& t) {
      return std::apply(std::forward<Fn>(f), std::forward<Tuple>(t));
    };
    // TODO why not this:
    // return std::bind_front(std::apply, std::forward<Fn>(f));
  }
};

inline constexpr auto apply_fn = _apply_fn{};

}  // namespace my_std

#endif  // MY_STD_HEADER_INCLUDED
