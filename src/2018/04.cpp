import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Event {
  enum {
    shift_begin,
    fall_asleep,
    wake_up,
  } type;
  int minute{};
  int guard{};
};

std::istream& operator>>(std::istream& is, Event& event) {
  using aoc::skip;
  using std::operator""s;
  if (int m, d, h, minute; is >> std::ws && skip(is, "[1518-"s) && is >> m && skip(is, "-"s)
                           && is >> d >> h && skip(is, ":"s) && is >> minute && skip(is, "]"s)) {
    if (std::string type; is >> type) {
      if (type == "Guard") {
        if (int guard; is >> std::ws && skip(is, "#"s) && is >> guard >> std::ws
                       && skip(is, "begins shift"s)) {
          event = {Event::shift_begin, minute, guard};
        }
      } else if (type == "falls") {
        if (is >> std::ws && skip(is, "asleep"s)) {
          event = {Event::fall_asleep, minute};
        }
      } else if (type == "wakes") {
        if (is >> std::ws && skip(is, "up"s)) {
          event = {Event::wake_up, minute};
        }
      } else {
        is.setstate(std::ios_base::failbit);
      }
    }
  }
  return is;
}

auto find_max(const auto& all_naps, const auto& f) {
  const auto [guard, naps]{*ranges::max_element(all_naps, ranges::less{}, [&](const auto& kv) {
    return f(kv.second);
  })};
  return guard * (ranges::max_element(naps) - naps.begin());
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0, std::plus{})};
constexpr auto max{std::__bind_back(my_std::ranges::fold_left, 0, ranges::max)};

auto find_sleepy_guards(const auto& events) {
  std::unordered_map<int, std::array<int, 60>> naps;
  for (const auto& e : events) {
    naps[e.guard].fill(0);
  }
  int nap_begin{}, guard{};
  for (const auto& e : events) {
    switch (e.type) {
      case Event::shift_begin: {
        guard = e.guard;
      } break;
      case Event::fall_asleep: {
        nap_begin = e.minute;
      } break;
      case Event::wake_up: {
        for (int t{nap_begin}; t < e.minute; ++t) {
          naps[guard][t] += 1;
        }
      } break;
    }
  }
  return std::pair{find_max(naps, sum), find_max(naps, max)};
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
  std::print("{} {}\n", part1, part2);
  return 0;
}
