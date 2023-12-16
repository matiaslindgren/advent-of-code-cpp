import std;

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;
struct IP {
  Strings supernets;
  Strings hypernets;
};

std::istream& operator>>(std::istream& is, IP& ip) {
  if (std::string line; is >> line) {
    Strings supernets;
    Strings hypernets;
    for (auto lhs{line.begin()}; lhs != line.end();) {
      const auto next_delim{*lhs == '[' ? ']' : '['};
      const auto rhs{ranges::find(lhs, line.end(), next_delim)};
      if (*lhs == '[' || *lhs == ']') {
        ++lhs;
      }
      if (lhs != line.end()) {
        (next_delim == ']' ? hypernets : supernets).emplace_back(lhs, rhs);
        lhs = rhs;
      }
    }
    ip = {supernets, hypernets};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing IP");
}

// TODO ranges::adjacent
constexpr decltype(auto) window3(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2));
}
constexpr decltype(auto) window4(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3));
}

bool contains_abba(std::string_view s) {
  return ranges::any_of(window4(s), [](const auto& window) {
    const auto& [ch1, ch2, ch3, ch4] = window;
    return ch1 != ch2 && ch1 == ch4 && ch2 == ch3;
  });
}

bool supports_tls(const IP& ip) {
  const auto has_super_abba{ranges::any_of(ip.supernets, contains_abba)};
  const auto has_hyper_abba{ranges::any_of(ip.hypernets, contains_abba)};
  return has_super_abba && !has_hyper_abba;
}

bool supports_ssl(const IP& ip) {
  std::vector<std::pair<char, char>> super_abas;
  for (const auto& s : ip.supernets) {
    for (const auto& [ch1, ch2, ch3] : window3(s)) {
      if (ch1 != ch2 && ch1 == ch3) {
        super_abas.push_back({ch1, ch2});
      }
    }
  }
  const auto has_super_aba{!super_abas.empty()};
  // clang-format off
  const auto has_hyper_bab{
    ranges::any_of(ip.hypernets, [&](const auto& s) {
      return ranges::any_of(window3(s), [&](const auto& window) {
        const auto& [ch1, ch2, ch3] = window;
        return (
          ch1 == ch3
          && ranges::any_of(super_abas, [&](const auto& ab) {
            const auto& [a, b] = ab;
            return a == ch2 && b == ch1;
          })
        );
      });
    })
  };
  // clang-format on
  return has_super_aba && has_hyper_bab;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto ips{views::istream<IP>(std::cin) | ranges::to<std::vector>()};

  const auto part1{ranges::count_if(ips, supports_tls)};
  const auto part2{ranges::count_if(ips, supports_ssl)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
