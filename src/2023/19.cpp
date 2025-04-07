#include "aoc.hpp"
#include "my_std.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;
using aoc::skip;

constexpr auto sum{std::bind_back(ranges::fold_left, 0UL, std::plus{})};
constexpr auto product{std::bind_back(ranges::fold_left, 1UL, std::multiplies{})};

enum struct Comparison : char {
  less = '<',
  greater = '>',
};

struct Rule {
  std::size_t i{};
  Comparison comp{};
  unsigned rhs{};
  std::string next;
};

struct Workflow {
  std::string id;
  std::vector<Rule> rules;
};

struct Rating {
  std::array<unsigned, 4> xmas{};

  [[nodiscard]]
  auto combination_count(const Rating& rhs) const {
    return product(
        views::transform(views::zip(xmas, rhs.xmas), my_std::apply_fn([](auto lo, auto up) {
                           return up - lo - 1;
                         }))
    );
  }

  bool operator<(const Rating& rhs) const {
    return ranges::all_of(views::zip(xmas, rhs.xmas), my_std::apply_fn(std::less{}));
  }
};

struct RatingBound {
  Rating lower{{0, 0, 0, 0}};
  Rating upper{{4001, 4001, 4001, 4001}};

  [[nodiscard]]
  bool contains(const Rating& r) const {
    return lower < r and r < upper;
  }

  [[nodiscard]]
  auto combination_count() const {
    return lower.combination_count(upper);
  }
};

auto find_bounds(const auto& workflows) {
  std::vector<RatingBound> bounds;
  for (std::deque q{{std::tuple{RatingBound{}, "in"s, 0UZ}}}; not q.empty(); q.pop_front()) {
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
      switch (rule.comp) {
        case Comparison::less: {
          branch_true.upper.xmas.at(i) = std::min(branch_true.upper.xmas.at(i), rhs);
          branch_false.lower.xmas.at(i) = std::max(branch_false.lower.xmas.at(i), rhs - 1);
        } break;
        case Comparison::greater: {
          branch_true.lower.xmas.at(i) = std::max(branch_true.lower.xmas.at(i), rhs);
          branch_false.upper.xmas.at(i) = std::min(branch_false.upper.xmas.at(i), rhs + 1);
        } break;
      }
      q.emplace_back(branch_true, rule.next, 0UZ);
      q.emplace_back(branch_false, workflow, i_rule + 1);
    }
  }
  return bounds;
}

auto find_part1(const auto& bounds, const auto& ratings) {
  return sum(
      views::filter(
          ratings,
          [&](const Rating& r) {
            return ranges::any_of(bounds, [&r](const RatingBound& b) { return b.contains(r); });
          }
      )
      | views::transform([&](const Rating& r) { return sum(r.xmas); })
  );
}

auto find_part2(const auto& bounds) {
  return sum(views::transform(bounds, [](const auto& b) { return b.combination_count(); }));
}

enum struct Variable : unsigned char {
  x = 0,
  m,
  a,
  s,
};

std::istream& operator>>(std::istream& is, Variable& var) {
  if (char ch{}; is >> ch) {
    switch (ch) {
      case 'x': {
        var = Variable::x;
      } break;
      case 'm': {
        var = Variable::m;
      } break;
      case 'a': {
        var = Variable::a;
      } break;
      case 's': {
        var = Variable::s;
      } break;
      default:
        throw std::runtime_error(std::format("unknown variable '{}'", ch));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Comparison& comp) {
  if (char ch{}; is >> ch) {
    if ("<>"s.contains(ch)) {
      comp = {ch};
    } else {
      throw std::runtime_error(std::format("unknown comparison '{}'", ch));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (std::string line; std::getline(is, line, ',') and not line.empty()) {
    if (not line.contains(':')) {
      line = "x<4001:" + line;
    }
    ranges::replace(line, ':', ' ');
    std::istringstream ls{line};
    if (auto [var, comp, rhs, next]{std::tuple{Variable{}, Comparison{}, 0U, ""s}};
        ls >> var >> comp >> rhs >> next) {
      rule = {std::to_underlying(var), comp, rhs, next};
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Workflow& workflow) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    ranges::replace(line, '{', ' ');
    ranges::replace(line, '}', ' ');
    std::istringstream ls{line};
    if (std::string id; ls >> id) {
      workflow = {
          .id = id,
          .rules = views::istream<Rule>(ls) | ranges::to<std::vector>(),
      };
      return is;
    }
  }
  throw std::runtime_error("failed parsing Workflow");
}

std::istream& operator>>(std::istream& is, Rating& r) {
  if (unsigned x{}, m{}, a{}, s{}; is >> std::ws >> skip("{"s) >> skip("x="s) >> x >> skip(",m="s)
                                   >> m >> skip(",a="s) >> a >> skip(",s="s) >> s >> skip("}"s)) {
    std::array xmas{x, m, a, s};
    if (0 <= ranges::min(xmas) and ranges::max(xmas) <= 4001) {
      r.xmas = xmas;
    } else {
      throw std::runtime_error("all xmas values must be in [0, 4001]");
    }
  } else if (not is.eof()) {
    throw std::runtime_error("failed parsing Rating");
  }
  return is;
}

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};

  std::unordered_map<std::string, Workflow> workflows;
  for (std::string line; std::getline(is, line) and not line.empty();) {
    std::istringstream ls{line};
    if (Workflow wf; ls >> wf) {
      workflows[wf.id] = wf;
    } else {
      throw std::runtime_error(std::format("failed parsing workflow from line '{}'", line));
    }
  }
  if (is.eof()) {
    throw std::runtime_error("input is missing the ratings section");
  }

  auto ratings{views::istream<Rating>(is) | ranges::to<std::vector>()};

  if (is.eof()) {
    return std::pair{workflows, ratings};
  }
  throw std::runtime_error("failed parsing input");
}

int main() {
  const auto [workflows, ratings]{parse_input("/dev/stdin")};

  const auto bounds{find_bounds(workflows)};

  const auto part1{find_part1(bounds, ratings)};
  const auto part2{find_part2(bounds)};

  std::println("{} {}", part1, part2);

  return 0;
}
