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
        std::decay_t<std::invoke_result_t<F &, T, std::iter_reference_t<I>>>;
    if (first == last) return U(std::move(init));
    U accum = std::invoke(f, std::move(init), *first);
    for (++first; first != last; ++first)
      accum = std::invoke(f, std::move(accum), *first);
    return std::move(accum);
  }

  template <std::ranges::input_range R, class T, class F>
  constexpr auto operator()(R &&r, T init, F f) const {
    return (*this)(std::ranges::begin(r),
                   std::ranges::end(r),
                   std::move(init),
                   std::ref(f));
  }
};

inline constexpr fold_left_fn fold_left;
}  // namespace ranges
}  // namespace my_std

struct Region {
  int top;
  int left;
  int bottom;
  int right;
  enum {
    turn_on,
    turn_off,
    toggle,
  } command;

  bool parse(std::istream &is) {
    std::string str;
    if (!(is >> str && (str == "turn" || str == "toggle"))) {
      return false;
    }
    if (str == "turn") {
      if (!(is >> str && (str == "on" || str == "off"))) {
        return false;
      }
      command = (str == "on") ? turn_on : turn_off;
    } else {
      command = toggle;
    }
    char ch;
    if (!(is >> top && is >> ch && ch == ',' && is >> left)) {
      return false;
    }
    if (!(is >> str && str == "through")) {
      return false;
    }
    if (!(is >> bottom && is >> ch && ch == ',' && is >> right)) {
      return false;
    }
    return true;
  }
};

std::ostream &operator<<(std::ostream &os, const Region &r) {
  std::string cmd;
  switch (r.command) {
    case Region::turn_on:
      cmd = "turn on";
      break;
    case Region::turn_off:
      cmd = "turn off";
      break;
    case Region::toggle:
      cmd = "toggle";
      break;
  }
  return os << "Region { t: " << r.top << " l: " << r.left << " b: " << r.bottom
            << " r: " << r.right << " cmd: " << cmd << " }";
}

std::istream &operator>>(std::istream &is, Region &r) {
  if (!r.parse(is) && !is.eof()) {
    throw std::runtime_error("failed parsing region");
  }
  return is;
}

int main() {
  const auto regions = std::views::istream<Region>(std::cin) |
                       std::ranges::to<std::vector<Region>>();
  std::vector<bool> part1_lights(1'000'000, false);
  std::vector<long long> part2_lights(1'000'000, 0);
  for (const auto &r : regions) {
    for (auto y{std::min(r.top, r.bottom)}; y <= std::max(r.top, r.bottom);
         ++y) {
      for (auto x{std::min(r.left, r.right)}; x <= std::max(r.left, r.right);
           ++x) {
        const auto current_brightness{part2_lights[y * 1000 + x]};
        switch (r.command) {
          case Region::turn_on: {
            part1_lights[y * 1000 + x] = true;
            part2_lights[y * 1000 + x] = current_brightness + 1;
          } break;
          case Region::turn_off: {
            part1_lights[y * 1000 + x] = false;
            part2_lights[y * 1000 + x] = std::max(0LL, current_brightness - 1);
          } break;
          case Region::toggle: {
            part1_lights[y * 1000 + x] = !part1_lights[y * 1000 + x];
            part2_lights[y * 1000 + x] = current_brightness + 2;
          } break;
        }
      }
    }
  }
  const auto part1{std::ranges::count_if(part1_lights, std::identity())};
  const auto part2{
      my_std::ranges::fold_left(part2_lights, 0LL, std::plus<long long>())};
  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
