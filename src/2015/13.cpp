import std;

// TODO(llvm18)
namespace my_std {
namespace ranges {
// Taken from "Possible implementations" at
// https://en.cppreference.com/w/cpp/algorithm/ranges/fold_left
// (accessed 2023-09-30)
struct fold_left_fn {
  template <std::input_iterator I, std::sentinel_for<I> S, class T, class F>
  constexpr auto operator()(I first, S last, T init, F f) const {
    using U
        = std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
    if (first == last) return U(std::move(init));
    U accum = std::invoke(f, std::move(init), *first);
    for (++first; first != last; ++first)
      accum = std::invoke(f, std::move(accum), *first);
    return std::move(accum);
  }

  template <std::ranges::input_range R, class T, class F>
  constexpr auto operator()(R&& r, T init, F f) const {
    return (*this)(std::ranges::begin(r),
                   std::ranges::end(r),
                   std::move(init),
                   std::ref(f));
  }
};

inline constexpr fold_left_fn fold_left;
}  // namespace ranges
}  // namespace my_std

namespace ranges = std::ranges;
namespace views = std::views;

struct Pair {
  std::string src;
  std::string dst;
  int happiness;
};

std::istream& operator>>(std::istream& is, Pair& p) {
  const auto skip = [&is](std::string_view s) {
    using std::operator""sv;
    return ranges::all_of(views::split(s, " "sv), [&is](auto&& w) {
      std::string tmp;
      return is >> tmp && tmp == std::string_view{w};
    });
  };
  std::string sign;
  if (is >> p.src && skip("would") && is >> sign
      && (sign == "gain" || sign == "lose") && is >> p.happiness
      && skip("happiness units by sitting next to") && is >> p.dst
      && !p.dst.empty() && p.dst.back() == '.') {
    if (sign == "lose") {
      p.happiness = -p.happiness;
    }
    p.dst.pop_back();
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing input");
}

class Graph {
  std::vector<std::string> nodes;
  std::vector<int> weights;

 public:
  std::size_t node_count() const {
    return nodes.size();
  }

  /* //TODO(P0847R7) */
  /* auto&& happiness(this auto&& self, auto n1, auto n2) { */
  /*   return weights[n1 + n2 * node_count()]; */
  /* } */
  auto& happiness(auto n1, auto n2) {
    return weights[n1 + n2 * node_count()];
  }
  const auto& happiness(auto n1, auto n2) const {
    return weights[n1 + n2 * node_count()];
  }

  explicit Graph(const std::vector<Pair>& pairs) {
    for (const auto& p : pairs) {
      nodes.push_back(p.src);
      nodes.push_back(p.dst);
    }
    {
      ranges::sort(nodes);
      const auto duplicates = ranges::unique(nodes);
      nodes.erase(duplicates.begin(), duplicates.end());
    }

    const auto n{node_count()};
    weights.resize(n * n);
    const auto find_node_index = [this](const auto& name) {
      return ranges::distance(this->nodes.begin(),
                              ranges::find(this->nodes, name));
    };
    for (const auto& p : pairs) {
      const auto n1{find_node_index(p.src)};
      const auto n2{find_node_index(p.dst)};
      happiness(n1, n2) = p.happiness;
    }
  }
};

constexpr auto accumulate = std::bind(my_std::ranges::fold_left,
                                      std::placeholders::_1,
                                      0,
                                      std::plus<int>());

// TODO rotated view?
auto find_seating_happiness(const auto& seating, const Graph& g) {
  const auto n{g.node_count()};
  return accumulate(views::transform(views::iota(0uz, n), [&](auto i) {
    const auto& lhs{seating[i]};
    const auto& rhs{seating[(i + 1) % n]};
    return g.happiness(lhs, rhs) + g.happiness(rhs, lhs);
  }));
}

auto maximize_happiness(const auto& pairs) {
  Graph g{pairs};
  auto seating = views::iota(0uz, g.node_count())
                 | ranges::to<std::vector<std::size_t>>();
  int happiness{};
  do {
    happiness = std::max(happiness, find_seating_happiness(seating, g));
  } while (ranges::next_permutation(seating).found);
  return happiness;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  auto pairs = views::istream<Pair>(std::cin) | ranges::to<std::vector<Pair>>();

  const auto part1{maximize_happiness(pairs)};

  pairs.emplace_back("Me", "Anyone", 0);
  const auto part2{maximize_happiness(pairs)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
