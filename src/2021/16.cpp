#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

using Bits = std::vector<bool>;
using Int = long;

constexpr auto sum{std::__bind_back(ranges::fold_left, Int{}, std::plus{})};
constexpr auto product{std::__bind_back(ranges::fold_left, Int{1}, std::multiplies{})};

Int as_int(ranges::sized_range auto&& bits) {
  return sum(bits | views::reverse | my_std::views::enumerate | views::transform([](auto&& ib) {
               auto [i, b]{ib};
               return (b ? Int{1} : Int{}) << i;
             }));
}

enum class Op : Int {
  sum,
  product,
  min,
  max,
  literal,
  greater,
  less,
  equal,
};

Op as_op(ranges::sized_range auto&& bits) {
  auto id{as_int(bits)};
  Op op{};
  switch (id) {
    case std::to_underlying(Op::sum):
    case std::to_underlying(Op::product):
    case std::to_underlying(Op::min):
    case std::to_underlying(Op::max):
    case std::to_underlying(Op::literal):
    case std::to_underlying(Op::greater):
    case std::to_underlying(Op::less):
    case std::to_underlying(Op::equal):
      op = {id};
      return op;
  }
  throw std::runtime_error(std::format("unknown op code {}", id));
}

// TODO libc++19?
auto chunks5(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1), views::drop(r, 2), views::drop(r, 3), views::drop(r, 4))
         | my_std::views::stride(5);
}

struct Packet;

struct Packet {
  Int version{};
  Int data{};
  Op op{};
  std::vector<Packet> packets;
  Int n_read{};

  explicit Packet(Bits bits) {
    version = as_int(bits | views::take(3));
    op = as_op(bits | views::drop(3) | views::take(3));
    bits = bits | views::drop(6) | ranges::to<Bits>();
    n_read = 6;
    if (op == Op::literal) {
      parse_literal(bits);
    } else {
      parse_subpackets(bits);
    }
  }

  void parse_literal(const Bits& bits) {
    Bits bits_data;
    for (auto [b1, b2, b3, b4, b5] : chunks5(bits)) {
      bits_data.append_range(std::array{b2, b3, b4, b5});
      n_read += 5;
      if (not b1) {
        break;
      }
    }
    data = as_int(bits_data);
  }

  void parse_subpackets(Bits bits) {
    if (bits.empty()) {
      throw std::runtime_error("empty subpacket list");
    }
    const Int header_len{bits.front() ? 11 : 15};
    Int limit{as_int(bits | views::drop(1) | views::take(header_len))};
    bits = bits | views::drop(1 + header_len) | ranges::to<Bits>();
    n_read += header_len + 1;
    Int pos{};
    while ((header_len == 15 and pos < limit) or (header_len == 11 and packets.size() < limit)) {
      Packet p(bits | views::drop(pos) | ranges::to<Bits>());
      packets.push_back(p);
      pos += p.n_read;
    }
    n_read += pos;
  }

  [[nodiscard]]
  Int version_sum() const {
    return version
           + sum(views::transform(packets, [](const Packet& p) { return p.version_sum(); }));
  }

  [[nodiscard]]
  Int eval() const {
    auto results{views::transform(packets, [](const Packet& p) { return p.eval(); })};
    switch (op) {
      case Op::sum:
        return sum(results);
      case Op::product:
        return product(results);
      case Op::min:
        return ranges::min(results);
      case Op::max:
        return ranges::max(results);
      case Op::literal:
        return data;
      case Op::greater:
        return int{results.front() > results.back()};
      case Op::less:
        return int{results.front() < results.back()};
      case Op::equal:
        return int{results.front() == results.back()};
    }
  }
};

Packet parse_packet(std::istream& is) {
  Bits bits;
  {
    std::string input;
    is >> input;
    for (auto it{input.begin()}; it != input.end(); ++it) {
      unsigned char buf{};
      if (std::from_chars(&it[0], &it[1], buf, 16).ec == std::errc{}) {
        for (int b{3}; b >= 0; --b) {
          bits.push_back(((buf >> b) & 1) != 0);
        }
      } else {
        throw std::runtime_error(std::format(
            "unknown input '{}', all non-whitespace input must be hexadecimal digits",
            *it
        ));
      }
    }
  }
  return Packet(bits);
}

int main() {
  std::ios::sync_with_stdio(false);
  const Packet p{parse_packet(std::cin)};

  const auto part1{p.version_sum()};
  const auto part2{p.eval()};

  std::println("{} {}", part1, part2);

  return 0;
}
