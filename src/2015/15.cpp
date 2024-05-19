#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""sv;

struct Ingredient {
  std::string name;
  int capacity;
  int durability;
  int flavor;
  int texture;
  int calories;
  constexpr std::vector<int> as_vector() const {
    return {capacity, durability, flavor, texture, calories};
  }
};

std::istream& operator>>(std::istream& is, Ingredient& ingredient) {
  using std::operator""s;
  using aoc::skip;
  if (Ingredient ing; is >> ing.name and ing.name.ends_with(":")
                      and is >> skip(" capacity"s) >> ing.capacity >> skip(", durability"s)
                              >> ing.durability >> skip(", flavor"s) >> ing.flavor
                              >> skip(", texture"s) >> ing.texture >> skip(", calories"s)
                              >> ing.calories) {
    ing.name.pop_back();
    ingredient = ing;
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Ingredient");
}

auto compute_scores(const auto& weights, const auto& spoons) {
  const auto inner_product_clamp_to_zero{[&spoons](const auto& w) {
    return std::max(0L, std::inner_product(w.begin(), w.end(), spoons.begin(), 0L));
  }};
  return views::transform(weights, inner_product_clamp_to_zero) | ranges::to<std::vector>();
}

constexpr auto product{std::bind(
    ranges::fold_left,
    std::placeholders::_1,
    std::placeholders::_2,
    1L,
    std::multiplies{}
)};

long find_optimal_cookie(
    const auto& weights,
    const auto& spoons,
    const std::optional<int>& calorie_target = {},
    const std::size_t& begin = {}
) {
  const auto scores{compute_scores(weights, spoons)};
  auto best_score{0L};
  if (not calorie_target or scores.back() == *calorie_target) {
    best_score = product(scores.begin(), scores.begin() + (weights.size() - 1));
  }
  for (auto i{begin}; i < spoons.size(); ++i) {
    if (spoons[i] == 0) {
      continue;
    }
    for (auto j{i + 1}; j < spoons.size(); ++j) {
      if (spoons[j] == 100) {
        continue;
      }
      for (auto next_spoons{spoons}; next_spoons[i];) {
        --next_spoons[i];
        ++next_spoons[j];
        const auto score{find_optimal_cookie(weights, next_spoons, calorie_target, j)};
        best_score = std::max(best_score, score);
      }
    }
  }
  return best_score;
}

int main() {
  const auto ingredients{aoc::parse_items<Ingredient>("/dev/stdin")};

  std::vector<std::vector<int>> weights{ingredients.front().as_vector().size()};
  for (const auto& ingredient : ingredients) {
    for (const auto& [w, i] : views::zip(weights, ingredient.as_vector())) {
      w.push_back(i);
    }
  }

  std::vector<int> init_spoons(ingredients.size());
  init_spoons.front() = 100;

  const auto part1{find_optimal_cookie(weights, init_spoons)};
  const auto part2{find_optimal_cookie(weights, init_spoons, {500})};

  std::print("{} {}\n", part1, part2);

  return 0;
}
