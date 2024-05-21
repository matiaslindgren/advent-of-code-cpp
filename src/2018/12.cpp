#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Rule {
  unsigned src;
  bool dst;
};

using Pots = std::vector<bool>;

constexpr auto bools2int{std::__bind_back(ranges::fold_left, 0u, [](auto x, bool b) {
  return (x << 1) | b;
})};
constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

std::istream& operator>>(std::istream& is, Pots& pots) {
  Pots p;
  if (std::string s; is >> s) {
    for (char c : s) {
      if (c == '#' or c == '.') {
        p.push_back(c == '#');
      } else {
        is.setstate(std::ios_base::failbit);
        break;
      }
    }
  }
  if (is or is.eof()) {
    pots = p;
    return is;
  }
  throw std::runtime_error("failed parsing Pots");
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (Pots src; is >> src >> std::ws >> skip("=>"s)) {
    if (Pots dst; is >> std::ws >> dst and dst.size() == 1) {
      rule = {bools2int(src), dst.front()};
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Rule");
}

auto parse_input(std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (Pots state; is >> skip("initial state:"s) >> state) {
    std::vector<int> rules(0b11111 + 1, 0);
    for (Rule r : views::istream<Rule>(is)) {
      if (r.dst) {
        rules.at(r.src) = r.dst + 1;
      }
    }
    return std::pair{state, rules};
  }
  throw std::runtime_error("failed parsing input state");
}

auto generate_plants(auto state, const auto& rules, const auto n_iter) {
  long zero{}, plants{}, delta{};
  for (int iter{}; iter < n_iter; ++iter) {
    while (ranges::any_of(state | views::take(5), std::identity{})) {
      zero -= 1;
      state.insert(state.begin(), false);
    }
    while (ranges::any_of(state | views::reverse | views::take(5), std::identity{})) {
      state.insert(state.end(), false);
    }
    const auto prev_state{std::exchange(state, Pots(state.size()))};
    for (auto i{0UZ}; i < state.size() - 5; ++i) {
      if (const auto pot{rules.at(bools2int(prev_state | views::drop(i) | views::take(5)))}) {
        state[i + 2] = pot - 1;
      }
    }
    const auto prev_plants{plants};
    plants = sum(
        my_std::views::enumerate(state, zero)
        | views::transform(my_std::apply_fn(std::multiplies{}))
    );
    if (delta == plants - prev_plants) {
      plants += (n_iter - iter - 1) * delta;
      break;
    }
    delta = plants - prev_plants;
  }
  return plants;
}

int main() {
  const auto [init_state, rules]{parse_input("/dev/stdin")};

  const auto part1{generate_plants(init_state, rules, 20)};
  const auto part2{generate_plants(init_state, rules, 50'000'000'000)};

  std::println("{} {}", part1, part2);

  return 0;
}
