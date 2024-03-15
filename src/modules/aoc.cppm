module;
import std;

export module aoc;

using std::operator""s;
using std::operator""sv;

export namespace aoc {

std::string slurp_file(std::string_view path) {
  std::ios::sync_with_stdio(false);

  std::ifstream is{path, std::ios::in | std::ios::binary};
  is.exceptions(std::ifstream::badbit);

  std::string data;
  for (std::array<char, 1024> buffer; is;) {
    is.read(buffer.data(), buffer.size());
    std::ranges::copy(buffer | std::views::take(is.gcount()), std::back_inserter(data));
  }
  return data;
}

auto cpu_count() {
  return std::max(1u, std::thread::hardware_concurrency());
};

template <typename... Patterns>
class skip {
  std::tuple<Patterns...> patterns;

  constexpr std::istream& skip_pattern(std::istream& is, auto&& p) const {
    if (is) {
      for (char rhs : p) {
        if (char lhs; not(is.get(lhs) and lhs == rhs)) {
          is.setstate(std::ios_base::failbit);
          break;
        }
      }
    }
    return is;
  }

  constexpr std::istream& skip_pattern(std::istream& is, auto&& p, auto&&... ps) const {
    if (skip_pattern(is, p)) {
      return (skip_pattern(is >> std::ws, ps), ...);
    }
    return is;
  }

 public:
  constexpr skip(Patterns&&... ps) : patterns(std::move(ps)...) {
  }

  constexpr std::istream& consume(std::istream& is) const {
    return std::apply(
        [&](auto&&... ps) -> std::istream& { return skip_pattern(is, ps...); },
        patterns
    );
  }
};

template <typename... Patterns>
std::istream& operator>>(std::istream& is, skip<Patterns...>&& s) {
  return s.consume(is);
}

template <std::integral Int>
constexpr Int saturating_add(Int a, Int b) {
  // https://stackoverflow.com/a/17582366/5951112
  // (2024-03-12)
  constexpr Int intmin{std::numeric_limits<Int>::min()};
  constexpr Int intmax{std::numeric_limits<Int>::max()};
  if (a > 0) {
    if (b > intmax - a) {
      return intmax;
    }
  } else if (b < intmin - a) {
    return intmin;
  }
  return a + b;
}

constexpr std::array<std::pair<std::string_view, char>, 33> ocr_letter_rows{{
    // 6-pixel character map adapted from
    // https://github.com/bsoyka/advent-of-code-ocr/blob/aae11d40720f0b681f2684b7aa4d25e3e0956b11/advent_of_code_ocr/characters.py
    // Accessed 2023-11-20
    {".##.#..##..######..##..#"sv, 'A'},
    {"###.#..####.#..##..####."sv, 'B'},
    {".##.#..##...#...#..#.##."sv, 'C'},
    {"#####...###.#...#...####"sv, 'E'},
    {"#####...###.#...#...#..."sv, 'F'},
    {".##.#..##...#.###..#.###"sv, 'G'},
    {"#..##..######..##..##..#"sv, 'H'},
    {".###..#...#...#...#..###"sv, 'I'},
    {"..##...#...#...##..#.##."sv, 'J'},
    {"#..##.#.##..#.#.#.#.#..#"sv, 'K'},
    {"#...#...#...#...#...####"sv, 'L'},
    {".##.#..##..##..##..#.##."sv, 'O'},
    {"###.#..##..####.#...#..."sv, 'P'},
    {"###.#..##..####.#.#.#..#"sv, 'R'},
    {".####...#....##....####."sv, 'S'},
    {"#..##..##..##..##..#.##."sv, 'U'},
    {"#...#....#.#..#...#...#."sv, 'Y'},
    {"####...#..#..#..#...####"sv, 'Z'},
    // 10-pixel character map adapted from
    // https://gist.github.com/usbpc/5fa0be48ad7b4b0594b3b8b029bc47b4
    // Accessed 2024-01-29
    {"..##...#..#.#....##....##....########....##....##....##....#"sv, 'A'},
    {"#####.#....##....##....######.#....##....##....##....######."sv, 'B'},
    {".####.#....##.....#.....#.....#.....#.....#.....#....#.####."sv, 'C'},
    {"#######.....#.....#.....#####.#.....#.....#.....#.....######"sv, 'E'},
    {"#######.....#.....#.....#####.#.....#.....#.....#.....#....."sv, 'F'},
    {".####.#....##.....#.....#.....#..####....##....##...##.###.#"sv, 'G'},
    {"#....##....##....##....########....##....##....##....##....#"sv, 'H'},
    {"...###....#.....#.....#.....#.....#.....#.#...#.#...#..###.."sv, 'J'},
    {"#....##...#.#..#..#.#...##....##....#.#...#..#..#...#.#....#"sv, 'K'},
    {"#.....#.....#.....#.....#.....#.....#.....#.....#.....######"sv, 'L'},
    {"#....###...###...##.#..##.#..##..#.##..#.##...###...###....#"sv, 'N'},
    {"#####.#....##....##....######.#.....#.....#.....#.....#....."sv, 'P'},
    {"#####.#....##....##....######.#..#..#...#.#...#.#....##....#"sv, 'R'},
    {"#....##....#.#..#..#..#...##....##...#..#..#..#.#....##....#"sv, 'X'},
    {"######.....#.....#....#....#....#....#....#.....#.....######"sv, 'Z'},
}};

constexpr char ocr(std::string_view rows) {
  if (const auto it{
          std::ranges::find_if(ocr_letter_rows, [&rows](auto&& p) { return p.first == rows; })
      };
      it != ocr_letter_rows.end()) {
    return it->second;
  }
  return ' ';
}

template <typename T, std::same_as<T>... Ts>
  requires(std::regular<T> and std::is_arithmetic_v<T>)
struct Vec {
  static constexpr std::size_t ndim{sizeof...(Ts) + 1};
  std::tuple<T, Ts...> elements;

  using value_type = T;
  using axes_indices = std::make_index_sequence<ndim>;

  constexpr Vec() = default;

  constexpr explicit Vec(value_type x, Ts... rest) : elements(x, rest...) {
  }

  // TODO deducing this
  template <std::size_t axis>
    requires(axis < ndim)
  constexpr value_type& get() noexcept {
    return std::get<axis>(elements);
  }
  template <std::size_t axis>
    requires(axis < ndim)
  constexpr const value_type& get() const noexcept {
    return std::get<axis>(elements);
  }

  // clang-format off
  // TODO deducing this
  constexpr value_type& x() noexcept { return get<0>(); }
  constexpr value_type& y() noexcept { return get<1>(); }
  constexpr value_type& z() noexcept { return get<2>(); }
  constexpr value_type& w() noexcept { return get<3>(); }
  constexpr const value_type& x() const noexcept { return get<0>(); }
  constexpr const value_type& y() const noexcept { return get<1>(); }
  constexpr const value_type& z() const noexcept { return get<2>(); }
  constexpr const value_type& w() const noexcept { return get<3>(); }
  // clang-format on

  [[nodiscard]] constexpr auto operator<=>(const Vec&) const = default;

 private:
  template <typename Fn, std::size_t... axes, typename... Args>
    requires((... and (axes < ndim)) and (... and std::same_as<Vec, std::decay_t<Args>>))
  constexpr Vec& apply_impl(Fn&& fn, std::index_sequence<axes...>, Args&&... args) noexcept {
    auto apply_on_axis{[&]<std::size_t axis>() { get<axis>() = fn(args.template get<axis>()...); }};
    (apply_on_axis.template operator()<axes>(), ...);
    return *this;
  }

 public:
  template <std::regular_invocable<value_type> UnaryFn>
  constexpr Vec& apply(UnaryFn&& fn) noexcept {
    return apply_impl(std::forward<UnaryFn>(fn), axes_indices{}, *this);
  }

  template <std::regular_invocable<value_type, value_type> BinaryFn>
  constexpr Vec& apply(BinaryFn&& fn, const Vec& rhs) noexcept {
    return apply_impl(std::forward<BinaryFn>(fn), axes_indices{}, *this, rhs);
  }

  // clang-format off
  constexpr Vec& operator+=(const Vec& rhs) noexcept { return apply(std::plus<value_type>{}, rhs); }
  constexpr Vec& operator-=(const Vec& rhs) noexcept { return apply(std::minus<value_type>{}, rhs); }
  constexpr Vec& operator*=(const Vec& rhs) noexcept { return apply(std::multiplies<value_type>{}, rhs); }
  constexpr Vec& operator/=(const Vec& rhs) noexcept { return apply(std::divides<value_type>{}, rhs); }
  // clang-format on

  [[nodiscard]] constexpr Vec operator+(const Vec& rhs) const noexcept {
    Vec lhs{*this};
    return lhs += rhs;
  }
  [[nodiscard]] constexpr Vec operator-(const Vec& rhs) const noexcept {
    Vec lhs{*this};
    return lhs -= rhs;
  }
  [[nodiscard]] constexpr Vec operator*(const Vec& rhs) const noexcept {
    Vec lhs{*this};
    return lhs *= rhs;
  }
  [[nodiscard]] constexpr Vec operator/(const Vec& rhs) const noexcept {
    Vec lhs{*this};
    return lhs /= rhs;
  }

  [[nodiscard]] constexpr Vec abs() const noexcept {
    Vec res{*this};
    return res.apply([](value_type val) noexcept -> value_type { return std::abs(val); });
  }

  [[nodiscard]] constexpr Vec signum() const noexcept {
    Vec res{*this};
    return res.apply([](value_type val) noexcept -> value_type {
      return (value_type{} < val) - (val < value_type{});
    });
  }

  [[nodiscard]] constexpr value_type sum() const noexcept {
    return std::apply(
        [](std::same_as<value_type> auto... vs) noexcept -> value_type { return (... + vs); },
        elements
    );
  }

  [[nodiscard]] constexpr value_type distance(const Vec& rhs) const noexcept {
    return (*this - rhs).abs().sum();
  }

  constexpr Vec& rotate_left() noexcept
    requires(ndim == 2)
  {
    x() = std::exchange(y(), -x());
    return *this;
  }

  constexpr Vec& rotate_right() noexcept
    requires(ndim == 2)
  {
    x() = -std::exchange(y(), x());
    return *this;
  }

  [[nodiscard]] constexpr auto adjacent() const noexcept
    requires(ndim == 2)
  {
    return std::array{
        *this - Vec(0, 1),
        *this - Vec(1, 0),
        *this + Vec(1, 0),
        *this + Vec(0, 1),
    };
  }
};

template <typename T>
using Vec1 = Vec<T>;

template <typename T>
using Vec2 = Vec<T, T>;

template <typename T>
using Vec3 = Vec<T, T, T>;

template <typename T>
using Vec4 = Vec<T, T, T, T>;

}  // namespace aoc

export template <std::integral... Ts>
struct std::hash<aoc::Vec<Ts...>> {
  using Vec = aoc::Vec<Ts...>;
  using T = Vec::value_type;
  static constexpr auto slot_width{std::numeric_limits<std::size_t>::digits / Vec::ndim};

  template <typename axes = Vec::axes_indices>
  constexpr auto operator()(const Vec& v) const noexcept {
    return [&]<std::size_t... axis>(std::index_sequence<axis...>) {
      return (... | (std::hash<T>{}(v.template get<axis>()) << (slot_width * axis)));
    }(axes{});
  }
};

export template <std::formattable<char>... Ts>
struct std::formatter<aoc::Vec<Ts...>, char> {
  using Vec = aoc::Vec<Ts...>;

  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Vec& v, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "Vec{}{}", Vec::ndim, v.elements);
  }
};

export template <typename... Ts>
std::ostream& operator<<(std::ostream& os, const aoc::Vec<Ts...>& v) {
  return os << std::format("{}", v);
}

export template <typename... Ts>
std::istream& operator>>(std::istream& is, aoc::Vec<Ts...>& v) {
  using Vec = aoc::Vec<Ts...>;

  if (Vec res; is >> res.template get<0>()) {
    [&]<std::size_t... axis>(std::index_sequence<axis...>) {
      ((is >> aoc::skip(","s) >> res.template get<axis + 1>()), ...);
    }(std::make_index_sequence<Vec::ndim - 1>{});
    if (is) {
      v = res;
    }
  }

  return is;
}
