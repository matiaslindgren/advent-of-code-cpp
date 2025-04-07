#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using Bits = std::bitset<12>;
constexpr std::size_t bit_count{Bits{}.size()};

Bits compute_gamma(const auto& nums) {
  std::array<int, bit_count> one_count{};
  for (const Bits& b : nums) {
    for (auto i{0UZ}; i < b.size(); ++i) {
      one_count.at(i) += b[i];
    }
  }
  Bits gamma;
  for (auto i{0UZ}; i < gamma.size(); ++i) {
    gamma[i] = 2 * one_count.at(i) >= nums.size();
  }
  return gamma;
}

auto multiply(const Bits& a, const Bits& b) {
  return a.to_ulong() * b.to_ulong();
}

auto find_part1(const auto& nums) {
  const Bits gamma{compute_gamma(nums)};
  const Bits epsilon{~gamma};
  return multiply(gamma, epsilon);
}

auto filter_bits(std::size_t i, const auto& nums, const Bits& mask) {
  return nums | views::filter([&](const Bits& bits) { return bits[i] == mask[i]; })
         | ranges::to<std::vector>();
}

auto find_part2(const auto& nums) {
  auto o2{nums};
  auto co2{nums};
  for (std::size_t i{bit_count - 1}; i < bit_count; --i) {
    if (o2.size() > 1) {
      const Bits gamma{compute_gamma(o2)};
      o2 = filter_bits(i, o2, gamma);
    }
    if (co2.size() > 1) {
      const Bits epsilon{~compute_gamma(co2)};
      co2 = filter_bits(i, co2, epsilon);
    }
    if (o2.size() == 1 and co2.size() == 1) {
      break;
    }
  }
  if (o2.empty() or co2.empty()) {
    throw std::runtime_error("o2 and co2 must not be empty");
  }
  return multiply(o2.front(), co2.front());
}

Bits parse_bitset(const std::string& s) {
  if (s.size() == bit_count and ranges::all_of(s, [](char ch) { return ch == '0' or ch == '1'; })) {
    return Bits(s);
  }
  throw std::runtime_error(
      std::format(
          "invalid bit string {}, must be of length {} and contain only 0s and 1s",
          bit_count,
          s
      )
  );
}

int main() {
  const auto nums{
      aoc::parse_items<std::string>("/dev/stdin") | views::transform(parse_bitset)
      | ranges::to<std::vector>()
  };

  const auto part1{find_part1(nums)};
  const auto part2{find_part2(nums)};
  std::println("{} {}", part1, part2);

  return 0;
}
