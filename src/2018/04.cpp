#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;

struct Event {
  enum : unsigned char {
    shift_begin,
    fall_asleep,
    wake_up,
  } type{};
  int minute{};
  int guard{};
};

auto find_max(const auto& all_naps, const auto& f) {
  const auto [guard, naps]{*ranges::max_element(all_naps, ranges::less{}, [&](const auto& kv) {
    return f(kv.second);
  })};
  return guard * (ranges::max_element(naps) - naps.begin());
}

constexpr auto sum{std::bind_back(ranges::fold_left, 0, std::plus{})};

auto find_sleepy_guards(const auto& events) {
  std::unordered_map<int, std::array<int, 60>> naps;
  for (const auto& e : events) {
    naps[e.guard].fill(0);
  }
  int nap_begin{};
  int guard{};
  for (const Event& e : events) {
    switch (e.type) {
      case Event::shift_begin: {
        guard = e.guard;
      } break;
      case Event::fall_asleep: {
        nap_begin = e.minute;
      } break;
      case Event::wake_up: {
        for (int t{nap_begin}; t < e.minute; ++t) {
          naps.at(guard).at(t) += 1;
        }
      } break;
    }
  }
  return std::pair{find_max(naps, sum), find_max(naps, ranges::max)};
}

std::istream& operator>>(std::istream& is, Event& event) {
  if (int m{}, d{}, h{}, minute{}; is >> std::ws >> skip("[1518-"s) >> m >> skip("-"s) >> d >> h
                                   >> skip(":"s) >> minute >> skip("]"s)) {
    if (std::string type; is >> type) {
      if (int guard{};
          type == "Guard"
          and is >> std::ws >> skip("#"s) >> guard >> std::ws >> skip("begins shift"s)) {
        event = {Event::shift_begin, minute, guard};
      } else if (type == "falls" and is >> std::ws >> skip("asleep"s)) {
        event = {Event::fall_asleep, minute};
      } else if (type == "wakes" and is >> std::ws >> skip("up"s)) {
        event = {Event::wake_up, minute};
      } else {
        throw std::runtime_error(std::format("failed parsing event of invalid type '{}'", type));
      }
    }
  }
  return is;
}

auto parse_events(std::string path) {
  std::vector<std::string> lines;
  {
    std::istringstream input{aoc::slurp_file(path)};
    for (std::string line; std::getline(input, line);) {
      lines.push_back(line);
    }
  }
  ranges::sort(lines);
  std::vector<Event> events;
  for (const auto& line : lines) {
    std::istringstream input{line};
    if (Event e; input >> e) {
      events.push_back(e);
    } else {
      throw std::runtime_error("failed parsing Event");
    }
  }
  return events;
}

int main() {
  const auto events{parse_events("/dev/stdin")};
  const auto [part1, part2]{find_sleepy_guards(events)};
  std::println("{} {}", part1, part2);
  return 0;
}
