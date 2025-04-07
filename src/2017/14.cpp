#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using HashState = std::array<uint8_t, 256>;
using Row = std::bitset<128>;

auto knot_hash(std::string input) {
  input.append_range(std::vector<char>{17, 31, 73, 47, 23});
  HashState state;
  // TODO (llvm21?) ranges::iota(state, 0u);
  for (unsigned x{}; x < state.size(); ++x) {
    state.at(x) = x;
  }
  unsigned long n{state.size()};
  unsigned long idx{};
  unsigned long skip{};
  for (int r{}; r < 64; ++r) {
    for (char len : input) {
      auto lhs{idx};
      auto rhs{(idx + len + n - 1) % n};
      for (int i{}; i < (len + 1) / 2; ++i) {
        std::swap(state.at(lhs), state.at(rhs));
        lhs = (lhs + 1) % n;
        rhs = (rhs + n - 1) % n;
      }
      idx = (idx + len + skip) % n;
      skip += 1;
    }
  }
  Row row;
  for (auto begin{0UL}; begin < state.size(); begin += 16) {
    const auto chunk{
        ranges::fold_left(state | views::drop(begin) | views::take(16), uint8_t{0}, std::bit_xor{})
    };
    row <<= 8;
    row |= Row(chunk);
  }
  return row;
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0U, std::plus{})};

auto parse_rows(std::string_view path) {
  auto lines{aoc::slurp_lines(path)};
  if (lines.empty()) {
    throw std::runtime_error("input must contain at least one line");
  }
  auto input{lines.front()};
  return (
      views::transform(
          views::iota(0, 128),
          [&input](const int row_idx) { return knot_hash(std::format("{}-{:d}", input, row_idx)); }
      )
      | ranges::to<std::vector<Row>>()
  );
}

auto compute_part1(const std::vector<Row>& rows) {
  return sum(views::transform(rows, [](const auto& row) { return row.count(); }));
}

auto bfs(const auto& rows, const auto y0, const auto x0) {
  std::unordered_set<int> visited;
  for (std::deque q{std::pair{y0, x0}}; not q.empty(); q.pop_front()) {
    auto [y, x]{q.front()};
    if (y >= rows.size() or x >= rows[y].size()) {
      continue;
    }
    const auto& row{rows.at(y)};
    const auto idx{y * row.size() + x};
    if (row[x] and not visited.contains(idx)) {
      visited.insert(idx);
      q.emplace_back(y, x - 1);
      q.emplace_back(y - 1, x);
      q.emplace_back(y, x + 1);
      q.emplace_back(y + 1, x);
    }
  }
  return visited;
}

auto compute_part2(const auto& rows) {
  std::unordered_set<int> visited;
  int regions{};
  for (auto&& [y, row] : my_std::views::enumerate(rows)) {
    for (auto x{0UL}; x < row.size(); ++x) {
      if (row[x] and not visited.contains(y * row.size() + x)) {
        visited.insert_range(bfs(rows, y, x));
        regions += 1;
      }
    }
  }
  return regions;
}

int main() {
  const auto rows{parse_rows("/dev/stdin")};

  const auto part1{compute_part1(rows)};
  const auto part2{compute_part2(rows)};

  std::println("{} {}", part1, part2);

  return 0;
}
