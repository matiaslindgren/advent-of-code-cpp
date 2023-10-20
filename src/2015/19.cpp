import std;

using Units = std::vector<std::string>;

struct Replacement {
  Units src;
  Units dst;
};

namespace ranges = std::ranges;
namespace views = std::views;

Units split_molecule(const std::string& molecule) {
  const auto islower{[](unsigned char ch) { return std::islower(ch); }};
  Units units;
  std::string unit;
  for (auto ch : molecule) {
    if (!islower(ch) && !unit.empty()) {
      units.push_back(unit);
      unit.clear();
    }
    unit.push_back(ch);
  }
  units.push_back(unit);
  return units;
}

std::ostream& operator<<(std::ostream& os, const Replacement& r) {
  os << "R { ";
  ranges::copy(r.src, std::ostream_iterator<std::string>(os, " "));
  os << " -> ";
  ranges::copy(r.dst, std::ostream_iterator<std::string>(os, " "));
  os << " }";
  return os;
}

std::istream& operator>>(std::istream& is, Replacement& r) {
  std::string tmp;
  std::string src;
  std::string dst;
  const auto isalpha{[](unsigned char ch) { return std::isalpha(ch); }};
  if (is >> src && ranges::all_of(src, isalpha) && is >> tmp && tmp == "=>" && is >> dst
      && ranges::all_of(dst, isalpha)) {
    r = {split_molecule(src), split_molecule(dst)};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Replacement");
}

std::vector<Units> replace(const Units& molecule, const Replacement& repl) {
  std::vector<Units> new_molecules;
  const auto begin{molecule.begin()};
  const auto end{molecule.end()};
  for (auto it = begin; it != end;) {
    const auto [match_begin, match_end] = ranges::search(ranges::subrange(it, end), repl.src);
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

std::vector<Units>
replace_all(const Units& molecule, const std::vector<Replacement>& replacements) {
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
    Units molecule,
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
    for (auto&& m : replace(molecule, r)) {
      if (new_molecule.empty() || m.size() < new_molecule.size()) {
        new_molecule = m;
      }
    }
  }
  return count_shortest_path_to(new_molecule, replacements, target, step + 1);
}

auto parse_input(std::istream& is) {
  std::vector<Replacement> replacements;
  std::string medicine;
  for (std::string line; std::getline(std::cin, line) && !line.empty();) {
    std::istringstream is{line};
    is >> replacements.emplace_back();
  }
  std::getline(std::cin, medicine);
  return std::make_pair(replacements, split_molecule(medicine));
}

int main() {
  std::ios_base::sync_with_stdio(false);
  using std::operator""s;

  const auto [replacements, medicine] = parse_input(std::cin);

  const auto part1{replace_all(medicine, replacements).size()};

  auto reverse_replacements = views::transform(
                                  replacements,
                                  [](auto&& r) -> Replacement {
                                    return {r.dst, r.src};
                                  }
                              )
                              | ranges::to<std::vector<Replacement>>();
  ranges::sort(reverse_replacements, ranges::greater{}, [](const auto& r) { return r.src.size(); });
  const auto part2{count_shortest_path_to(medicine, reverse_replacements, {"e"s})};

  std::print("{} {}\n", part1, part2);

  return 0;
}
