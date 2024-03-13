import std;
import aoc;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Quantity {
  int n;
  std::string chem;
};

struct Recipe {
  Quantity output;
  std::vector<Quantity> inputs;
};

std::istream& operator>>(std::istream& is, Quantity& q) {
  if (int n; is >> n) {
    if (std::string chem; is >> chem) {
      q = {n, chem};
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Recipe& recipe) {
  Recipe r;
  if (std::string line; std::getline(is, line) and not line.empty()) {
    ranges::replace(line, ',', ' ');
    const auto parts{
        views::split(line, "=>"s)
        | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
        | ranges::to<std::vector<std::string>>()
    };
    if (parts.size() != 2) {
      throw std::runtime_error("recipe should contain two parts separated by '=>'");
    }
    std::istringstream ls{parts[0]};
    r.inputs = views::istream<Quantity>(ls) | ranges::to<std::vector>();
    ls = std::istringstream(parts[1]);
    if (ls >> r.output) {
      recipe = r;
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  return is;
}

auto find_ore_amount(const auto& recipes, const long fuel_target) {
  std::unordered_map<std::string, long> need{{"FUEL"s, fuel_target}};

  const auto pending_reaction{[](auto&& item) { return item.first != "ORE"s and item.second > 0; }};

  for (auto it{need.begin()}; it != need.end(); it = ranges::find_if(need, pending_reaction)) {
    const Recipe& r{recipes.at(it->first)};
    auto& target{it->second};
    auto n_reactions{(target + r.output.n - 1) / r.output.n};
    target -= r.output.n * n_reactions;
    for (const auto& input : r.inputs) {
      need[input.chem] += input.n * n_reactions;
    }
  }

  return need.at("ORE"s);
}

auto find_part1(const auto& recipes) {
  return find_ore_amount(recipes, 1);
}

auto find_part2(const auto& recipes) {
  const auto max_ore{1'000'000'000'000L};
  for (auto fuel{0L};;) {
    const auto ore{find_ore_amount(recipes, fuel + 1)};
    if (ore > max_ore) {
      return fuel;
    }
    fuel = std::max(fuel + 1, (fuel + 1) * max_ore / ore);
  }
}

auto parse_input(const std::string path) {
  std::unordered_map<std::string, Recipe> recipes;
  std::istringstream input{aoc::slurp_file(path)};
  for (Recipe r : views::istream<Recipe>(input)) {
    recipes[r.output.chem] = r;
  }
  if (input.eof()) {
    return recipes;
  }
  throw std::runtime_error("invalid input, parsing failed");
}

int main() {
  const auto recipes{parse_input("/dev/stdin")};

  const auto part1{find_part1(recipes)};
  const auto part2{find_part2(recipes)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
