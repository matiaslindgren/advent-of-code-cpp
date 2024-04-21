#include "std.hpp"
#include "aoc.hpp"
#include "my_std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Range {
  int begin{}, end{};

  bool contains(int val) const {
    return begin <= val and val <= end;
  }
};

struct Rule {
  std::string name;
  Range a, b;

  bool is_valid(int val) const {
    return a.contains(val) or b.contains(val);
  }
};

struct Ticket {
  std::vector<int> values;
};

std::istream& operator>>(std::istream& is, Range& range) {
  if (int begin, end; is >> begin >> skip("-"s) >> end) {
    range = {begin, end};
  }
  return is;
}

std::istream& operator>>(std::istream& is, Rule& rule) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    bool ok{false};
    if (auto sep{ranges::find(line, ':')}; sep != line.end()) {
      std::string name(line.begin(), sep);
      std::string values(sep + 1, line.end());
      if (not name.empty() and not values.empty()) {
        std::stringstream ls{values};
        if (Range a, b; ls >> a >> std::ws >> skip("or"s) >> b) {
          rule = {.name = name, .a = a, .b = b};
          ok = true;
        }
      }
    }
    if (not ok) {
      throw std::runtime_error(std::format("failed parsing line {} as rule", line));
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Ticket& ticket) {
  if (std::string line; std::getline(is, line) and not line.empty()) {
    ranges::replace(line, ',', ' ');
    std::stringstream ls{line};
    ticket = {views::istream<int>(ls) | ranges::to<std::vector>()};
    if (not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line {} as ticket", line));
    }
  }
  return is;
}

auto search(const auto& rules, const auto& my_ticket, auto tickets) {
  int error_rate{};

  for (const auto& t : std::exchange(tickets, {})) {
    bool ok{true};
    for (int val : t.values) {
      if (not ranges::any_of(rules, [&val](const auto& rule) { return rule.is_valid(val); })) {
        error_rate += val;
        ok = false;
        break;
      }
    }
    if (ok) {
      tickets.push_back(t);
    }
  }

  if (tickets.empty()) {
    throw std::runtime_error("valid tickets should not be empty");
  }

  std::unordered_map<int, std::unordered_map<int, bool>> valid_rules;
  for (auto i{0uz}; i < tickets.front().values.size(); ++i) {
    for (auto [r, rule] : my_std::views::enumerate(rules)) {
      for (const auto& t : tickets) {
        auto& vr{valid_rules[i]};
        if (not vr.contains(r)) {
          vr[r] = true;
        }
        vr[r] = vr[r] and rule.is_valid(t.values[i]);
      }
    }
  }

  std::unordered_map<int, Rule> pos2rule;
  for (std::unordered_set<int> taken; taken.size() < rules.size();) {
    for (auto [i, valid] : valid_rules) {
      int n_free{}, prev{};
      for (auto r : valid | views::filter([](auto item) { return item.second; }) | views::keys) {
        if (not taken.contains(r)) {
          n_free += 1;
          prev = r;
        }
      }
      if (n_free == 1) {
        pos2rule[i] = rules[prev];
        taken.insert(prev);
      }
    }
  }

  long part2{1};
  for (auto [i, val] : my_std::views::enumerate(my_ticket.values)) {
    if (pos2rule[i].name.starts_with("departure"s)) {
      part2 *= val;
    }
  }

  return std::pair{error_rate, part2};
}

auto parse_input(std::string path) {
  const auto input{aoc::slurp_file(path)};
  const auto sections{
      views::split(input, "\n\n"s)
      | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
      | ranges::to<std::vector<std::string>>()
  };
  if (sections.size() != 3) {
    throw std::runtime_error("input must contain 3 sections separated by 2 newlines");
  }

  std::istringstream is{sections[0]};
  auto rules{views::istream<Rule>(is) | ranges::to<std::vector>()};
  if (is.eof()) {
    is = std::istringstream(sections[1]);
    if (is >> std::ws >> skip("your ticket:"s) >> std::ws) {
      if (Ticket my_ticket; is >> my_ticket) {
        is = std::istringstream(sections[2]);
        if (is >> std::ws >> skip("nearby tickets:"s) >> std::ws) {
          auto tickets{views::istream<Ticket>(is) | ranges::to<std::vector>()};
          if (is.eof()) {
            return std::tuple{rules, my_ticket, tickets};
          }
        }
      }
    }
  }
  throw std::runtime_error("failed parsing input sections");
}

int main() {
  const auto [rules, my_ticket, tickets]{parse_input("/dev/stdin")};
  const auto [part1, part2]{search(rules, my_ticket, tickets)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
