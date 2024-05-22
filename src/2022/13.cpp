#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_digit;
using std::operator""s;

struct Packet {
  std::string str;

  explicit operator int() const {
    return std::atoi(str.c_str());
  }

  bool is_numeric() const {
    return ranges::all_of(str, [=](unsigned char ch) { return is_digit(ch); });
  }

  std::vector<Packet> split() const {
    if (is_numeric()) {
      return {*this};
    }
    std::vector<Packet> subpacks;
    for (auto [i, depth, size]{std::tuple{1UZ, 0UZ, 0UZ}}; i < str.size(); ++i) {
      const char ch{str.at(i)};
      depth += int{ch == '['};
      depth -= int{ch == ']'};
      if ((ch == ',' and depth == 0) or i == str.size() - 1) {
        if (size) {
          subpacks.emplace_back(str.substr(i - size, size));
          size = 0;
        }
      } else {
        size += 1;
      }
    }
    return subpacks;
  }

  auto operator<=>(const Packet& rhs) const {
    if (is_numeric() and rhs.is_numeric()) {
      return int(*this) <=> int(rhs);
    }
    auto lhs_packets{split()};
    auto rhs_packets{rhs.split()};
    for (auto [lhs2, rhs2] : views::zip(lhs_packets, rhs_packets)) {
      if (auto cmp{lhs2 <=> rhs2}; cmp != 0) {
        return cmp;
      }
    }
    return lhs_packets.size() <=> rhs_packets.size();
  }

  bool operator==(const Packet& rhs) const {
    return (*this <=> rhs) == 0;
  }
};

// TODO (llvm19) ranges::chunks
auto chunks2(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1)) | my_std::views::stride(2);
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

auto find_part1(const auto& packets) {
  return sum(views::transform(
      my_std::views::enumerate(chunks2(packets), 1),
      [](auto&& item) -> long {
        auto [i, packs]{item};
        auto [lhs, rhs]{packs};
        return int{lhs < rhs} * i;
      }
  ));
}

auto find_part2(auto packets) {
  const Packet div2{"[[2]]"s};
  const Packet div6{"[[6]]"s};
  packets.append_range(std::array{div2, div6});
  ranges::sort(packets);
  auto pos2{ranges::find(packets, div2)};
  auto pos6{ranges::find(packets, div6)};
  return (pos2 - packets.begin() + 1) * (pos6 - packets.begin() + 1);
}

std::istream& operator>>(std::istream& is, Packet& packet) {
  if (std::string line; is >> line and not line.empty()) {
    if (ranges::all_of(line, [](char ch) { return "[],0123456789"s.contains(ch); })) {
      packet.str = line;
    } else {
      throw std::runtime_error(std::format("invalid packet '{}'", line));
    }
  }
  return is;
}

int main() {
  const auto packets{aoc::parse_items<Packet>("/dev/stdin")};

  if (packets.empty() or packets.size() % 2) {
    throw std::runtime_error("input must contain an even number of packets");
  }

  const auto part1{find_part1(packets)};
  const auto part2{find_part2(packets)};

  std::println("{} {}", part1, part2);

  return 0;
}
