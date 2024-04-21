#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Rule {
  std::size_t i;
  char op;
  unsigned rhs;
  std::string next;
};

struct Workflow {
  std::string id;
  std::vector<Rule> rules;
};

using Rating = std::array<unsigned, 4>;

struct RatingBound {
  Rating lower{{0, 0, 0, 0}};
  Rating upper{{4001, 4001, 4001, 4001}};

  constexpr bool contains(const Rating& r) const {
    auto less{my_std::apply_fn(ranges::less{})};
    return ranges::all_of(views::zip(lower, r), less)
           and ranges::all_of(views::zip(r, upper), less);
  }

  constexpr auto combination_count() const {
    return my_std::ranges::fold_left(
        views::zip(lower, upper)
            | views::transform(my_std::apply_fn([](auto&& lo, auto&& up) { return up - lo - 1; })),
        1uz,
        std::multiplies{}
    );
  }
};

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (std::string line; std::getline(is, line, ',') and not line.empty()) {
    if (not line.contains(':')) {
      line = "x<4001:" + line;
    }
    ranges::replace(line, ':', ' ');
    std::stringstream ls{line};
    if (char var; ls >> var and "xmas"s.contains(var)) {
      const auto i{var == 'x' ? 0u : var == 'm' ? 1u : var == 'a' ? 2u : 3u};
      if (char op; ls >> op and "<>"s.contains(op)) {
        if (unsigned rhs; ls >> rhs) {
          if (std::string next; ls >> next) {
            rule = {i, op, rhs, next};
            return is;
          }
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Rule");
}

std::istream& operator>>(std::istream& is, Workflow& workflow) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    ranges::replace(line, '{', ' ');
    ranges::replace(line, '}', ' ');
    std::stringstream ls{line};
    if (std::string id; ls >> id) {
      workflow = {
          .id = id,
          .rules = views::istream<Rule>(ls) | ranges::to<std::vector>(),
      };
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Workflow");
}

auto parse_workflows(std::istream& is) {
  std::unordered_map<std::string, Workflow> workflows;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    std::stringstream ls{line};
    if (Workflow wf; ls >> wf) {
      workflows[wf.id] = wf;
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return workflows;
  }
  throw std::runtime_error("failed parsing workflows");
}

auto parse_ratings(std::istream& is) {
  using aoc::skip;
  std::vector<Rating> ratings;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    std::stringstream ls{line};
    if (ls >> skip("{"s)) {
      if (unsigned x, m, a, s;
          ls >> skip("x="s) >> x >> skip(",m="s) >> m >> skip(",a="s) >> a >> skip(",s="s) >> s) {
        if (ls >> skip("}"s)) {
          ratings.push_back({x, m, a, s});
        }
      }
    }
    if (not ls) {
      throw std::runtime_error("failed parsing Rating");
    }
  }
  if (is or is.eof()) {
    return ratings;
  }
  throw std::runtime_error("failed parsing ratings");
}

auto parse_input(std::istream& is) {
  auto workflows{parse_workflows(is)};
  auto ratings{parse_ratings(is)};
  if (is.eof()) {
    return std::pair(workflows, ratings);
  }
  throw std::runtime_error("failed parsing input");
}

auto find_bounds(const auto& workflows) {
  std::vector<RatingBound> bounds;
  for (std::deque q{{std::tuple{RatingBound{}, "in"s, 0uz}}}; not q.empty(); q.pop_front()) {
    const auto [bound, workflow, i_rule]{q.front()};
    if (workflow == "A"s) {
      bounds.push_back(bound);
      continue;
    }
    if (workflow == "R"s) {
      continue;
    }
    if (const auto& rules{workflows.at(workflow).rules}; i_rule < rules.size()) {
      const auto& rule{rules[i_rule]};
      const auto i{rule.i};
      const auto rhs{rule.rhs};
      auto branch_true{bound};
      auto branch_false{bound};
      if (rule.op == '<') {
        branch_true.upper[i] = std::min(branch_true.upper[i], rhs);
        branch_false.lower[i] = std::max(branch_false.lower[i], rhs - 1);
      } else {
        branch_true.lower[i] = std::max(branch_true.lower[i], rhs);
        branch_false.upper[i] = std::min(branch_false.upper[i], rhs + 1);
      }
      q.emplace_back(branch_true, rule.next, 0uz);
      q.emplace_back(branch_false, workflow, i_rule + 1);
    }
  }
  return bounds;
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0uz, std::plus{})};

auto find_part1(const auto& bounds, const auto& ratings) {
  return sum(
      views::filter(
          ratings,
          [&](const auto& r) {
            return ranges::any_of(bounds, [&r](const auto& b) { return b.contains(r); });
          }
      )
      | views::transform([](const auto& r) { return sum(r); })
  );
}

auto find_part2(const auto& bounds) {
  return sum(views::transform(bounds, [](const auto& b) { return b.combination_count(); }));
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto [workflows, ratings]{parse_input(input)};

  const auto bounds{find_bounds(workflows)};

  const auto part1{find_part1(bounds, ratings)};
  const auto part2{find_part2(bounds)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
