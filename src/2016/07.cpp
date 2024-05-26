#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;
using std::operator""s;

// TODO (llvm19?) ranges::adjacent
constexpr decltype(auto) window3(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2));
}
constexpr decltype(auto) window4(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3));
}

bool contains_abba(std::string_view s) {
  // clang-format off
  return ranges::any_of(
    window4(s),
    my_std::apply_fn([](char c1, char c2, char c3, char c4) {
      return c1 != c2 and c1 == c4 and c2 == c3;
    })
  );
  // clang-format on
}

struct IP {
  Strings supernets;
  Strings hypernets;
};

bool supports_tls(const IP& ip) {
  return ranges::any_of(ip.supernets, contains_abba)
         and not ranges::any_of(ip.hypernets, contains_abba);
}

bool supports_ssl(const IP& ip) {
  std::vector<std::pair<char, char>> super_abas;
  for (const std::string& s : ip.supernets) {
    for (const auto& [c1, c2, c3] : window3(s)) {
      if (c1 != c2 and c1 == c3) {
        super_abas.emplace_back(c1, c2);
      }
    }
  }
  bool has_super_aba{not super_abas.empty()};
  // clang-format off
  bool has_hyper_bab{
    ranges::any_of(
      ip.hypernets,
      [&](const std::string& s) {
        return ranges::any_of(
          window3(s),
          my_std::apply_fn([&](char c1, char c2, char c3) {
            return (
              c1 == c3
              and ranges::any_of(
                super_abas,
                my_std::apply_fn([&](char a, char b) { return a == c2 and b == c1; })
              )
            );
          })
        );
      }
    )
  };
  // clang-format on
  return has_super_aba and has_hyper_bab;
}

std::istream& operator>>(std::istream& is, IP& ip) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    Strings supernets;
    Strings hypernets;
    line = "]"s + line;
    for (bool in_bracket{}; char ch : line) {
      if (ch == '[') {
        in_bracket = true;
        hypernets.emplace_back();
      } else if (ch == ']') {
        in_bracket = false;
        supernets.emplace_back();
      } else if (in_bracket) {
        hypernets.back().push_back(ch);
      } else {
        supernets.back().push_back(ch);
      }
    }
    ip = {supernets, hypernets};
  }
  return is;
}

int main() {
  const auto ips{aoc::parse_items<IP>("/dev/stdin")};

  const auto part1{ranges::count_if(ips, supports_tls)};
  const auto part2{ranges::count_if(ips, supports_ssl)};

  std::println("{} {}", part1, part2);

  return 0;
}
