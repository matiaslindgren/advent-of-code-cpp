import std;

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

std::istream& operator>>(std::istream& is, Ingredient& i) {
  const auto skip = [&is](std::string_view s) {
    return ranges::all_of(views::split(s, " "sv), [&is](auto&& w) {
      std::string tmp;
      return is >> tmp && tmp == std::string_view{w};
    });
  };
  if (is >> i.name && i.name.ends_with(":") && skip("capacity")
      && is >> i.capacity && skip(", durability") && is >> i.durability
      && skip(", flavor") && is >> i.flavor && skip(", texture")
      && is >> i.texture && skip(", calories") && is >> i.calories) {
    i.name.pop_back();
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Ingredient");
}

auto compute_scores(const auto& weights, const auto& spoons) {
  const auto inner_product_clamp_to_zero{[&spoons](const auto& w) {
    return std::max(
        0LL,
        std::inner_product(w.begin(), w.end(), spoons.begin(), 0LL));
  }};
  return views::transform(weights, inner_product_clamp_to_zero)
         | ranges::to<std::vector<long long>>();
}

long long find_optimal_cookie(const auto& weights,
                              const auto& spoons,
                              const std::optional<int>& calorie_target = {},
                              const std::size_t& begin = {}) {
  const auto scores{compute_scores(weights, spoons)};
  auto best_score{0LL};
  if (!calorie_target || scores.back() == *calorie_target) {
    best_score
        = std::accumulate(scores.begin(),
                          ranges::next(scores.begin(), weights.size() - 1),
                          1LL,
                          std::multiplies<long long>());
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
        const auto score{
            find_optimal_cookie(weights, next_spoons, calorie_target, j)};
        best_score = std::max(best_score, score);
      }
    }
  }
  return best_score;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto ingredients = views::istream<Ingredient>(std::cin)
                           | ranges::to<std::vector<Ingredient>>();

  std::vector<std::vector<int>> weights{ingredients.front().as_vector().size()};
  for (const auto& ingredient : ingredients) {
    for (auto&& [w, i] : views::zip(weights, ingredient.as_vector())) {
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