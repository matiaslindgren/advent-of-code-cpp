module;
import std;

export module aoc;

using std::operator""s;

const std::unordered_map<std::string, char> aoc_letter_to_ascii{
    // 6-pixel character map adapted from
    // https://github.com/bsoyka/advent-of-code-ocr/blob/aae11d40720f0b681f2684b7aa4d25e3e0956b11/advent_of_code_ocr/characters.py
    // Accessed 2023-11-20
    {".##.#..##..######..##..#"s, 'A'},
    {"###.#..####.#..##..####."s, 'B'},
    {".##.#..##...#...#..#.##."s, 'C'},
    {"#####...###.#...#...####"s, 'E'},
    {"#####...###.#...#...#..."s, 'F'},
    {".##.#..##...#.###..#.###"s, 'G'},
    {"#..##..######..##..##..#"s, 'H'},
    {".###..#...#...#...#..###"s, 'I'},
    {"..##...#...#...##..#.##."s, 'J'},
    {"#..##.#.##..#.#.#.#.#..#"s, 'K'},
    {"#...#...#...#...#...####"s, 'L'},
    {".##.#..##..##..##..#.##."s, 'O'},
    {"###.#..##..####.#...#..."s, 'P'},
    {"###.#..##..####.#.#.#..#"s, 'R'},
    {".####...#....##....####."s, 'S'},
    {"#..##..##..##..##..#.##."s, 'U'},
    {"#...#....#.#..#...#...#."s, 'Y'},
    {"####...#..#..#..#...####"s, 'Z'},
    // 10-pixel character map adapted from
    // https://gist.github.com/usbpc/5fa0be48ad7b4b0594b3b8b029bc47b4
    // Accessed 2024-01-29
    {"..##...#..#.#....##....##....########....##....##....##....#"s, 'A'},
    {"#####.#....##....##....######.#....##....##....##....######."s, 'B'},
    {".####.#....##.....#.....#.....#.....#.....#.....#....#.####."s, 'C'},
    {"#######.....#.....#.....#####.#.....#.....#.....#.....######"s, 'E'},
    {"#######.....#.....#.....#####.#.....#.....#.....#.....#....."s, 'F'},
    {".####.#....##.....#.....#.....#..####....##....##...##.###.#"s, 'G'},
    {"#....##....##....##....########....##....##....##....##....#"s, 'H'},
    {"...###....#.....#.....#.....#.....#.....#.#...#.#...#..###.."s, 'J'},
    {"#....##...#.#..#..#.#...##....##....#.#...#..#..#...#.#....#"s, 'K'},
    {"#.....#.....#.....#.....#.....#.....#.....#.....#.....######"s, 'L'},
    {"#....###...###...##.#..##.#..##..#.##..#.##...###...###....#"s, 'N'},
    {"#####.#....##....##....######.#.....#.....#.....#.....#....."s, 'P'},
    {"#####.#....##....##....######.#..#..#...#.#...#.#....##....#"s, 'R'},
    {"#....##....#.#..#..#..#...##....##...#..#..#..#.#....##....#"s, 'X'},
    {"######.....#.....#....#....#....#....#....#.....#.....######"s, 'Z'},
};

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

template <typename Int>
  requires(std::integral<Int>)
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

char ocr(const std::string& aoc_letter) {
  if (const auto it{aoc_letter_to_ascii.find(aoc_letter)}; it != aoc_letter_to_ascii.end()) {
    return it->second;
  }
  return ' ';
}

template <typename T, typename... Ts>
  requires((std::regular<T> and std::is_arithmetic_v<T>) and ... and std::same_as<T, Ts>)
struct Vec {
  using value_type = T;
  using container_type = std::tuple<value_type, Ts...>;
  static constexpr std::size_t ndim{sizeof...(Ts) + 1};

  container_type elements;

  constexpr Vec() = default;

  constexpr explicit Vec(value_type x, Ts... rest) : elements(x, rest...) {
  }

  // TODO deducing this
  template <std::size_t I>
    requires(I < ndim)
  constexpr value_type& get() noexcept {
    return std::get<I>(elements);
  }
  template <std::size_t I>
    requires(I < ndim)
  constexpr const value_type& get() const noexcept {
    return std::get<I>(elements);
  }

  // TODO deducing this
  constexpr value_type& x() noexcept {
    return get<0>();
  }
  constexpr value_type& y() noexcept {
    return get<1>();
  }
  constexpr value_type& z() noexcept {
    return get<2>();
  }
  constexpr value_type& w() noexcept {
    return get<3>();
  }
  constexpr const value_type& x() const noexcept {
    return get<0>();
  }
  constexpr const value_type& y() const noexcept {
    return get<1>();
  }
  constexpr const value_type& z() const noexcept {
    return get<2>();
  }
  constexpr const value_type& w() const noexcept {
    return get<3>();
  }

  [[nodiscard]] constexpr auto operator<=>(const Vec&) const = default;

  template <std::size_t I = ndim - 1, typename BinaryFn>
    requires(std::regular_invocable<BinaryFn, value_type, value_type>)
  constexpr Vec& apply(const Vec& rhs, BinaryFn&& fn) {
    get<I>() = fn(get<I>(), rhs.get<I>());
    if constexpr (I) {
      apply<I - 1, BinaryFn>(rhs, std::forward<BinaryFn>(fn));
    }
    return *this;
  }

  template <std::size_t I = ndim - 1, typename UnaryFn>
    requires(std::regular_invocable<UnaryFn, value_type>)
  constexpr Vec& apply(UnaryFn&& fn) {
    get<I>() = fn(get<I>());
    if constexpr (I) {
      apply<I - 1, UnaryFn>(std::forward<UnaryFn>(fn));
    }
    return *this;
  }

  constexpr Vec& operator+=(const Vec& rhs) noexcept {
    return apply(rhs, std::plus<value_type>{});
  }

  constexpr Vec& operator-=(const Vec& rhs) noexcept {
    return apply(rhs, std::minus<value_type>{});
  }

  constexpr Vec& operator*=(const Vec& rhs) noexcept {
    return apply(rhs, std::multiplies<value_type>{});
  }

  constexpr Vec& operator/=(const Vec& rhs) noexcept {
    return apply(rhs, std::divides<value_type>{});
  }

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
    return res.apply([](auto&& x) noexcept -> value_type { return std::abs(x); });
  }

  [[nodiscard]] constexpr value_type sum() const noexcept {
    return std::apply([](auto&&... vs) noexcept -> value_type { return (... + vs); }, elements);
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

export template <typename... Ts>
  requires(... and std::integral<Ts>)
struct std::hash<aoc::Vec<Ts...>> {
  using Vec = aoc::Vec<Ts...>;
  constexpr auto operator()(const Vec& v) const noexcept {
    auto slot_width{std::numeric_limits<typename Vec::value_type>::digits / Vec::ndim};
    return std::invoke(
        [&]<std::size_t... i>(std::index_sequence<i...>) {
          return (
              ...
              | (std::hash<typename Vec::value_type>{}(std::get<i>(v.elements)) << (slot_width * i))
          );
        },
        std::make_index_sequence<Vec::ndim>{}
    );
  }
};

export template <std::formattable<char>... Ts>
struct std::formatter<aoc::Vec<Ts...>, char> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const aoc::Vec<Ts...>& v, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "Vec{}{}", sizeof...(Ts), v.elements);
  }
};

export template <typename... Ts>
std::ostream& operator<<(std::ostream& os, const aoc::Vec<Ts...>& v) {
  return os << std::format("{}", v);
}

export template <typename... Ts>
std::istream& operator>>(std::istream& is, aoc::Vec<Ts...>& v) {
  if (aoc::Vec<Ts...> res; is >> std::get<0>(res.elements)) {
    std::invoke(
        [&is, &res]<std::size_t... i>(std::index_sequence<i...>) {
          ((is >> aoc::skip(","s) >> std::get<i + 1>(res.elements)), ...);
        },
        std::make_index_sequence<sizeof...(Ts) - 1>{}
    );
    if (is) {
      v = res;
    }
  }
  return is;
}
