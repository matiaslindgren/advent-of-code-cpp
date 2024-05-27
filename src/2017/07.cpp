#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Node {
  std::string id;
  int weight{};
  std::vector<std::string> children;

  [[nodiscard]]
  bool is_leaf() const {
    return children.empty();
  }
};

std::istream& operator>>(std::istream& is, Node& node) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::istringstream ls{line};
    if (std::string id; ls >> id) {
      if (int weight{}; ls >> std::ws >> skip("("s) >> weight >> skip(")"s)) {
        if (ls >> std::ws >> skip("->"s)) {
          node = {id, weight, views::istream<std::string>(ls) | ranges::to<std::vector>()};
        } else {
          node = {id, weight};
        }
      }
    }
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {}", line));
    }
  }
  return is;
}

auto find_parents(const auto& nodes) {
  auto parent{
      views::zip(views::keys(nodes), views::repeat(""s)) | ranges::to<std::unordered_map>()
  };
  for (const Node& n : nodes | views::values) {
    for (const auto& child : n.children) {
      parent[child] = n.id;
    }
  }
  return parent;
}

auto find_root(const auto& nodes) {
  const auto parent{find_parents(nodes)};
  for (const Node& n : nodes | views::values) {
    if (parent.at(n.id).empty()) {
      return n;
    }
  }
  throw std::runtime_error("cannot find root");
}

constexpr auto sum{std::__bind_back(ranges::fold_left, 0, std::plus{})};

int find_tree_weight(const auto& root, const auto& nodes) {
  return root.weight + sum(views::transform(root.children, [&nodes](const auto& cid) {
           return find_tree_weight(nodes.at(cid), nodes);
         }));
}

auto find_balance_weight(const auto& nodes) {
  const auto weight{
      nodes | views::values | views::transform([&nodes](const auto& n) {
        return std::pair{n.id, find_tree_weight(n, nodes)};
      })
      | ranges::to<std::unordered_map<std::string, int>>()
  };

  const auto parent{find_parents(nodes)};

  int res{std::numeric_limits<int>::max()};
  int minw{std::numeric_limits<int>::max()};

  for (const auto& leaf :
       nodes | views::values | views::filter([](const auto& n) { return n.is_leaf(); })) {
    for (auto id{parent.at(leaf.id)}; not id.empty(); id = parent.at(id)) {
      const auto& n{nodes.at(id)};
      std::unordered_map<int, int> wfreq;
      for (const auto& child : n.children) {
        wfreq[weight.at(child)] += 1;
      }
      if (wfreq.size() == 1) {
        continue;
      }
      const auto unbalanced{ranges::find_if(wfreq, [](const auto& p) { return p.second == 1; })};
      const auto balanced{ranges::find_if(wfreq, [](const auto& p) { return p.second > 1; })};
      if (unbalanced == wfreq.end() or balanced == wfreq.end()) {
        continue;
      }
      const auto w_ub{unbalanced->first};
      const auto w_b{balanced->first};
      if (w_ub < minw) {
        const auto n_ub{*ranges::find_if(n.children, [&](const auto& child) {
          return weight.at(child) == w_ub;
        })};
        minw = w_ub;
        res = nodes.at(n_ub).weight + (w_b - w_ub);
      }
    }
  }

  return res;
}

int main() {
  const auto nodes{
      aoc::parse_items<Node>("/dev/stdin")
      | views::transform([](const auto& n) { return std::pair{n.id, n}; })
      | ranges::to<std::unordered_map>()
  };

  const auto part1{find_root(nodes).id};
  const auto part2{find_balance_weight(nodes)};

  std::println("{} {}", part1, part2);

  return 0;
}
