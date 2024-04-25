#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Seeds = std::vector<long>;

struct Range {
  long idx;
  long len;
  constexpr auto operator<=>(const Range&) const = default;
  constexpr auto lhs() const {
    return idx;
  }
  constexpr auto rhs() const {
    return idx + len;
  }
};
using Ranges = std::vector<Range>;

struct Map {
  Range dst;
  Range src;
};
using MapGroups = std::vector<std::vector<Map>>;

std::istream& operator>>(std::istream& is, Seeds& ss) {
  if (std::string line; is >> skip("seeds:"s) and std::getline(is, line)) {
    std::stringstream ls{line};
    ss = views::istream<long>(ls) | ranges::to<std::vector>();
    return is;
  }
  throw std::runtime_error("failed parsing Seeds");
}

std::istream& operator>>(std::istream& is, MapGroups& mg) {
  const std::array names{
      "seed-to-soil"s,
      "soil-to-fertilizer"s,
      "fertilizer-to-water"s,
      "water-to-light"s,
      "light-to-temperature"s,
      "temperature-to-humidity"s,
      "humidity-to-location"s,
  };
  for (const auto& map_name : names) {
    mg.emplace_back();
    if (std::string tmp; is >> tmp and tmp == map_name and is >> skip(" map:\n"s)) {
      for (std::string line; std::getline(is, line) and not line.empty();) {
        std::stringstream ls{line};
        if (long dst, src, len; ls >> dst >> src >> len) {
          mg.back().push_back({.dst = {dst, len}, .src = {src, len}});
        } else {
          is.setstate(std::ios_base::failbit);
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing MapGroups");
}

constexpr void prune(Ranges& rs) {
  {
    const auto empty{ranges::remove_if(rs, [](const auto& r) { return r.len <= 0; })};
    rs.erase(empty.begin(), empty.end());
  }
  ranges::sort(rs);
  {
    const auto dup{ranges::unique(rs)};
    rs.erase(dup.begin(), dup.end());
  }
}

Ranges split_subranges(const Range& input, const auto& map_groups) {
  Ranges subranges{{input}};

  for (const auto& maps : map_groups) {
    for (const auto& subrange : std::exchange(subranges, Ranges{})) {
      Ranges unmapped{{subrange}};
      for (const auto& map : maps) {
        for (const auto& r : std::exchange(unmapped, Ranges{})) {
          const auto src{map.src};

          if (r.rhs() < src.lhs() or src.rhs() < r.lhs()) {
            unmapped.push_back(r);
            continue;
          }

          const auto dst{map.dst};
          const auto min_rhs{std::min(r.rhs(), src.rhs())};

          if (r < src) {
            unmapped.push_back({r.lhs(), src.lhs() - r.lhs()});
            subranges.push_back({dst.lhs(), min_rhs - src.lhs()});
            unmapped.push_back({src.rhs(), r.rhs() - src.rhs()});
          } else if (r > src) {
            subranges.push_back({dst.lhs() + r.lhs() - src.lhs(), min_rhs - r.lhs()});
            unmapped.push_back({src.rhs(), r.rhs() - src.rhs()});
          } else {
            subranges.push_back(dst);
          }
        }
      }
      subranges.append_range(unmapped);
      prune(subranges);
    }
  }

  return subranges;
}

auto search(const Ranges& seeds, const auto& map_groups) {
  auto min_idx{std::numeric_limits<long>::max()};
  for (const auto& input : seeds) {
    const auto& subranges{split_subranges(input, map_groups)};
    min_idx = std::min(min_idx, ranges::min_element(subranges)->idx);
  }
  return min_idx;
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  Seeds seeds;
  MapGroups map_groups;
  input >> seeds >> map_groups;

  const auto seed_ranges1{
      seeds | views::transform([](auto idx) { return Range{idx, 1}; }) | ranges::to<Ranges>()
  };
  const auto part1{search(seed_ranges1, map_groups)};

  const auto chunks2{[](const auto& r) {
    return views::zip(r, views::drop(r, 1)) | my_std::views::stride(2);
  }};
  const auto seed_ranges2{
      chunks2(seeds) | views::transform([](const auto& chunk) {
        const auto& [idx, len] = chunk;
        return Range{idx, len};
      })
      | ranges::to<Ranges>()
  };
  const auto part2{search(seed_ranges2, map_groups)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
