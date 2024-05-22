#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::is_alpha;
using aoc::is_lower;
using aoc::skip;
using std::operator""s;
using Units = std::vector<std::string>;

struct Replacement {
  Units src;
  Units dst;
};

Units split_molecule(const std::string& molecule) {
  Units units;
  std::string unit;
  for (auto ch : molecule) {
    if (not is_lower(ch) and not unit.empty()) {
      units.push_back(unit);
      unit.clear();
    }
    unit.push_back(ch);
  }
  units.push_back(unit);
  return units;
}

auto replace(const Units& molecule, const Replacement& repl) {
  std::vector<Units> new_molecules;
  const auto begin{molecule.begin()};
  const auto end{molecule.end()};
  for (auto it{begin}; it != end;) {
    const auto [match_begin, match_end]{ranges::search(ranges::subrange(it, end), repl.src)};
    if (match_begin != end) {
      const auto back_inserter{std::back_inserter(new_molecules.emplace_back())};
      ranges::copy(begin, match_begin, back_inserter);
      ranges::copy(repl.dst, back_inserter);
      ranges::copy(match_end, end, back_inserter);
    }
    it = match_end;
  }
  return new_molecules;
}

auto replace_all(const Units& molecule, const std::vector<Replacement>& replacements) {
  std::vector<Units> new_molecules;
  for (const auto& r : replacements) {
    ranges::copy(replace(molecule, r), std::back_inserter(new_molecules));
  }
  ranges::sort(new_molecules);
  const auto [dup_begin, dup_end] = ranges::unique(new_molecules);
  new_molecules.erase(dup_begin, dup_end);
  return new_molecules;
}

std::size_t count_shortest_path_to(
    const Units& molecule,
    const std::vector<Replacement>& replacements,
    const Units& target,
    std::size_t step = 0
) {
  if (molecule == target) {
    return step;
  }
  if (molecule.empty()) {
    return std::numeric_limits<std::size_t>::max();
  }
  // greedy works?? hmm lazy
  Units new_molecule;
  for (const auto& r : replacements) {
    for (const auto& m : replace(molecule, r)) {
      if (new_molecule.empty() or m.size() < new_molecule.size()) {
        new_molecule = m;
      }
    }
  }
  return count_shortest_path_to(new_molecule, replacements, target, step + 1);
}

auto parse_input(std::string_view path) {
  const auto lines{aoc::slurp_lines(path)};
  if (lines.size() < 3) {
    throw std::runtime_error("expected at least 3 lines");
  }

  std::vector<Replacement> replacements;
  for (auto line : lines | views::take(lines.size() - 2)) {
    std::istringstream ls{line};
    if (std::string src, dst; ls >> src and ranges::all_of(src, is_alpha)
                              and ls >> std::ws >> skip("=>"s) >> dst
                              and ranges::all_of(dst, is_alpha)) {
      replacements.emplace_back(split_molecule(src), split_molecule(dst));
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line '{}'", line));
    }
  }

  const auto& medicine{lines.back()};
  return std::pair{replacements, split_molecule(medicine)};
}

int main() {
  const auto [replacements, medicine]{parse_input("/dev/stdin")};

  const auto part1{replace_all(medicine, replacements).size()};

  auto reverse_replacements{
      views::transform(replacements, [](const auto& r) { return Replacement{r.dst, r.src}; })
      | ranges::to<std::vector>()
  };
  ranges::sort(reverse_replacements, ranges::greater{}, [](const auto& r) { return r.src.size(); });
  const auto part2{count_shortest_path_to(medicine, reverse_replacements, {"e"s})};

  std::println("{} {}", part1, part2);

  return 0;
}
