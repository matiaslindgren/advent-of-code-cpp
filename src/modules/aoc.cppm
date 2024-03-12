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
  } else if (b < intmax - a) {
    return intmin;
  }
  return a + b;
}

char as_ascii(std::string aoc_letter) {
  // given an advent of code ascii art letter in row-major order,
  // return the corresponding character
  if (const auto it{aoc_letter_to_ascii.find(aoc_letter)}; it != aoc_letter_to_ascii.end()) {
    return it->second;
  }
  return ' ';
}

template <typename T, typename... Ts>
  requires(std::is_arithmetic_v<T> and ... and std::same_as<T, Ts>)
struct Vec {
  using value_type = T;
  using container_type = std::tuple<value_type, Ts...>;

  container_type elements;

  constexpr Vec() = default;

  constexpr explicit Vec(T x, Ts... rest) : elements(std::make_tuple(x, rest...)) {
  }

  // TODO deducing this
  template <std::size_t I>
    requires(I <= sizeof...(Ts))
  constexpr value_type& get() noexcept {
    return std::get<I>(elements);
  }
  template <std::size_t I>
    requires(I <= sizeof...(Ts))
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

  template <std::size_t I = sizeof...(Ts), typename BinaryFn>
    requires(std::regular_invocable<BinaryFn, value_type, value_type>)
  constexpr Vec& apply(const Vec& rhs, BinaryFn&& fn) {
    get<I>() = fn(get<I>(), rhs.get<I>());
    if constexpr (I) {
      apply<I - 1, BinaryFn>(rhs, std::forward<BinaryFn>(fn));
    }
    return *this;
  }

  template <std::size_t I = sizeof...(Ts), typename UnaryFn>
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
    requires(sizeof...(Ts) == 1)
  {
    y() = std::exchange(x(), -y());
    return *this;
  }

  constexpr Vec& rotate_right() noexcept
    requires(sizeof...(Ts) == 1)
  {
    y() = -std::exchange(x(), y());
    return *this;
  }

  /*
  [[nodiscard]] constexpr auto adjacent() const noexcept {
    return std::array{
        Vec(x() - 1, y()),
        Vec(x(), y() - 1),
        Vec(x(), y() + 1),
        Vec(x() + 1, y()),
    };
  }
  */
};

template <typename T>
using Vec1 = Vec<T>;

template <typename T>
using Vec2 = Vec<T, T>;

template <typename T>
using Vec3 = Vec<T, T, T>;

template <typename T>
using Vec4 = Vec<T, T, T, T>;

#if 0
template <typename T>
std::istream& operator>>(std::istream& is, Vec2<T>& vec) {
  using std::operator""s;
  if (T x, y; is >> x >> std::ws and is >> skip(","s) >> y) {
    vec = Vec2(x, y);
  }
  return is;
}
#endif

}  // namespace aoc

template <typename T, typename... Ts>
  requires(std::integral<T> and ... and std::same_as<T, Ts>)
struct std::hash<aoc::Vec<T, Ts...>> {
  static constexpr T width{std::numeric_limits<T>::digits / (sizeof...(Ts) + 1)};

  template <std::size_t I = sizeof...(Ts)>
  constexpr auto operator()(const aoc::Vec<T, Ts...>& v) const noexcept {
    if constexpr (auto x{std::hash<T>{}(std::get<I>(v.elements))}; I == 0) {
      return x;
    } else {
      return (x << (width * I)) | this->operator()<I - 1>(v);
    }
  }
};

template <std::formattable<char>... Ts>
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
