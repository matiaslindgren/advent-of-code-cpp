#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Ring {
  using List = std::list<long>;
  using Iter = List::iterator;
  List data;
  Iter head;
  std::unordered_map<List::value_type, Iter> nodes;

  Ring(const auto& values, const std::size_t size) : data(size) {
    int i{};
    for (auto pos{data.begin()}; pos != data.end(); ++pos, ++i) {
      if (i < values.size()) {
        *pos = values[i];
      } else {
        *pos = i + 1;
      }
      nodes[*pos] = pos;
    }
    head = data.begin();
  }

  auto next(auto it) {
    ranges::advance(it, 1, data.end());
    if (it == data.end()) {
      it = data.begin();
    }
    return it;
  }

  void step() {
    const auto cup1{*head};
    {
      const auto n{ranges::ssize(data)};
      auto cup2{*head};
      List pick;
      for (auto it{next(head)}; pick.size() < 3;) {
        pick.splice(pick.end(), data, std::exchange(it, next(it)));
      }
      while (cup1 == cup2 or ranges::contains(pick, cup2)) {
        cup2 = (cup2 - 2 + n) % n + 1;
      }
      data.splice(next(nodes.at(cup2)), pick);
    }
    head = next(nodes.at(cup1));
  }

  auto to_str() {
    std::ostringstream os;
    for (auto it{next(nodes.at(1))}; *it != 1; it = next(it)) {
      os << *it;
    }
    return os.str();
  }
};

Ring simulate(const auto& input, std::size_t n, std::size_t steps) {
  Ring ring(input, n);
  for (std::size_t i{}; i < steps; ++i) {
    ring.step();
  }
  return ring;
}

auto find_part1(const auto& input) {
  return simulate(input, input.size(), 100).to_str();
}

auto find_part2(const auto& input) {
  Ring r{simulate(input, 1'000'000, 10'000'000)};
  auto one{r.nodes.at(1)};
  long long n1{*r.next(one)};
  long long n2{*r.next(r.next(one))};
  return n1 * n2;
}

int char2int(char ch) {
  if (aoc::is_digit(ch)) {
    return ch - '0';
  }
  throw std::runtime_error("all non-whitespace input must be digits");
}

auto parse_digits(std::string_view path) {
  return aoc::parse_items<char>(path) | views::transform(char2int) | ranges::to<std::vector>();
}

int main() {
  const auto input{parse_digits("/dev/stdin")};
  const auto part1{find_part1(input)};
  const auto part2{find_part2(input)};
  std::println("{} {}", part1, part2);
  return 0;
}
