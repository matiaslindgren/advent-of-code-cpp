import std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Aunt {
  using Items = std::unordered_map<std::string, int>;
  std::size_t id;
  Items items;
};

std::istream& operator>>(std::istream& is, Aunt& aunt) {
  std::string tmp;
  std::size_t id;
  if (is >> tmp && tmp == "Sue" && is >> id >> tmp && tmp == ":") {
    Aunt::Items items;
    {
      std::string key;
      int value;
      while (is >> key && key.ends_with(":") && is >> value) {
        key.pop_back();
        items[key] = value;
        if (!(is.peek() == ',' && is.get())) {
          break;
        }
      }
    }
    aunt = {id, items};
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Aunt");
}

Aunt find_aunt(std::vector<Aunt> aunts, const Aunt& target, auto comp) {
  for (const auto& [k, v] : target.items) {
    const auto is_mismatch
        = [&](const auto& aunt) { return aunt.items.contains(k) && !comp(k, aunt.items.at(k), v); };
    const auto [rm_begin, rm_end] = ranges::remove_if(aunts, is_mismatch);
    aunts.erase(rm_begin, rm_end);
  }
  return aunts.front();
}

int main() {
  std::ios_base::sync_with_stdio(false);

  const auto aunts = views::istream<Aunt>(std::cin) | ranges::to<std::vector<Aunt>>();

  const Aunt target{
      .items
      = {{"children", 3},
         {"cats", 7},
         {"samoyeds", 2},
         {"pomeranians", 3},
         {"akitas", 0},
         {"vizslas", 0},
         {"goldfish", 5},
         {"trees", 3},
         {"cars", 2},
         {"perfumes", 1}}
  };

  const auto equal_compare{[](const auto&, int lhs, int rhs) { return lhs == rhs; }};

  const auto fancy_compare{[](const auto& key, int lhs, int rhs) {
    if (key == "cats" || key == "trees") {
      return lhs > rhs;
    }
    if (key == "pomerians" || key == "goldfish") {
      return lhs < rhs;
    }
    return lhs == rhs;
  }};

  const auto aunt1{find_aunt(aunts, target, equal_compare)};
  const auto aunt2{find_aunt(aunts, target, fancy_compare)};

  std::print("{} {}\n", aunt1.id, aunt2.id);

  return 0;
}
