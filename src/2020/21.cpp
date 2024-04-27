#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::set<std::string>;

struct Food {
  Strings ingredients;
  Strings allergens;
};

using Foods = std::vector<Food>;

std::istream& operator>>(std::istream& is, Food& food) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    auto rm{ranges::remove_if(line, [](char ch) { return ",()"s.contains(ch); })};
    line.erase(rm.begin(), rm.end());
    Food f;
    std::istringstream ls{line};
    for (std::string ig; ls >> ig and ig != "contains"s;) {
      f.ingredients.insert(ig);
    }
    for (std::string ag; ls >> ag and not ag.empty();) {
      f.allergens.insert(ag);
    }
    if (not ls and not ls.eof()) {
      throw std::runtime_error("failed parsing Food");
    }
    food = f;
  }
  return is;
}

Strings intersect_ingredients(const Foods& foods) {
  if (foods.empty()) {
    throw std::runtime_error("food list must not be empty");
  }
  auto common{foods.at(0).ingredients};
  for (const Food& f : foods | views::drop(1)) {
    const auto prev{std::exchange(common, {})};
    std::ranges::set_intersection(prev, f.ingredients, std::inserter(common, common.end()));
  }
  return common;
}

using AllergenMap = std::unordered_map<std::string, Strings>;

bool is_unique(const AllergenMap& am) {
  if (am.empty()) {
    return false;
  }
  if (ranges::any_of(views::values(am), [](const Strings& s) { return s.size() != 1; })) {
    return false;
  }
  Strings seen;
  for (const Strings& igs : views::values(am)) {
    if (auto [_, unseen]{seen.insert(*igs.begin())}; not unseen) {
      return false;
    }
  }
  return true;
}

AllergenMap find_allergen_mapping(
    const AllergenMap& full_map,
    const AllergenMap& mapping,
    const Strings& allergens
) {
  if (allergens.empty()) {
    return mapping;
  }
  const std::string ag{*allergens.begin()};
  for (const std::string& ig : full_map.at(ag)) {
    auto next_mapping{mapping};
    next_mapping[ag] = {ig};

    auto next_allergens{allergens};
    next_allergens.erase(ag);

    if (auto candidate{find_allergen_mapping(full_map, next_mapping, next_allergens)};
        is_unique(candidate)) {
      return candidate;
    }
  }
  return {};
}

auto search(const Foods& foods) {
  std::unordered_map<std::string, std::set<std::size_t>> allergen2foods;
  for (auto [id, food] : my_std::views::enumerate(foods)) {
    for (const std::string& ag : food.allergens) {
      allergen2foods[ag].insert(id);
    }
  }

  Strings has_allergen;
  AllergenMap allergen2ingredients;
  for (auto [ag, food_ids] : allergen2foods) {
    for (auto ig : intersect_ingredients(
             views::transform(food_ids, [&](auto i) { return foods.at(i); }) | ranges::to<Foods>()
         )) {
      has_allergen.insert(ig);
      allergen2ingredients[ag].insert(ig);
    }
  }

  int part1{};
  for (const Food& f : foods) {
    for (const auto& ig : f.ingredients) {
      part1 += not has_allergen.contains(ig);
    }
  }

  Strings all_allergens{allergen2foods | views::keys | ranges::to<Strings>()};
  auto mapping{
      find_allergen_mapping(allergen2ingredients, {}, all_allergens)
      | views::transform([](auto&& kv) {
          auto [ag, igs]{kv};
          return std::pair{ag, *igs.begin()};
        })
      | ranges::to<std::vector>()
  };
  ranges::sort(mapping);
  // TODO (llvm19?) views::join_with(mapping|views::values, ',')|ranges::to<std::string>()
  std::string part2;
  for (auto ig : mapping | views::values) {
    if (not part2.empty()) {
      part2.push_back(',');
    }
    part2 += ig;
  }

  return std::pair{part1, part2};
}

int main() {
  const auto foods{aoc::slurp<Food>("/dev/stdin")};
  const auto [part1, part2]{search(foods)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
