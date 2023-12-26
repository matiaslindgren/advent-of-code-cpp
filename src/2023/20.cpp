import std;
import aoc;
import my_std;

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;

struct Module {
  enum class Type {
    Broadcaster,
    FlipFlop,
    Conjunction,
    Output,
  } type;
  std::string id;
  Strings inputs;
  Strings outputs;
  std::unordered_map<std::string, bool> high;
};

using Modules = std::unordered_map<std::string, Module>;

std::istream& operator>>(std::istream& is, Module::Type& t) {
  using Type = Module::Type;
  const char ch = is.peek();
  switch (ch) {
    case 'b': {
      t = Type::Broadcaster;
      return is;
    } break;
    case '%': {
      if (is.get() == ch) {
        t = Type::FlipFlop;
        return is;
      }
    } break;
    case '&': {
      if (is.get() == ch) {
        t = Type::Conjunction;
        return is;
      }
    } break;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Module::Type");
}

std::istream& operator>>(std::istream& is, Module& m) {
  if (std::string line; std::getline(is, line)) {
    ranges::replace(line, ',', ' ');
    std::stringstream ls{line};
    if (Module::Type type; ls >> type) {
      if (std::string id; ls >> id && !id.empty()) {
        if (ls >> std::ws && skip(ls, "->"s)) {
          const auto outputs{views::istream<std::string>(ls) | ranges::to<Strings>()};
          m = {.type = type, .id = id, .outputs = outputs};
          return is;
        }
      }
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Module");
}

auto parse_input(std::istream& is) {
  return views::istream<Module>(is) | views::transform([](auto&& m) {
           return std::pair{m.id, m};
         })
         | ranges::to<Modules>();
}

auto update_module_connections(auto modules) {
  std::unordered_map<std::string, Strings> all_inputs;
  for (const auto& [id, m] : modules) {
    for (const auto& o : m.outputs) {
      all_inputs[o].push_back(id);
    }
  }
  for (const auto& [id, inputs] : all_inputs) {
    if (!modules.contains(id)) {
      modules[id] = Module{.type = Module::Type::Output, .id = id};
    }
    auto& m{modules[id]};
    m.inputs = inputs;
    switch (m.type) {
      case Module::Type::FlipFlop: {
        m.high[m.id] = false;
      } break;
      case Module::Type::Conjunction: {
        for (const auto& i : m.inputs) {
          m.high[i] = false;
        }
      } break;
      default: {
      } break;
    }
  }
  return modules;
}

auto search(auto modules) {
  const auto all_conjunctions{[&modules](const auto& ms) {
    return !ms.empty() && ranges::all_of(ms, [&modules](const auto& m) {
      return modules.at(m).type == Module::Type::Conjunction;
    });
  }};
  Strings rx_inputs{modules.at("rx"s).inputs};
  if (rx_inputs.size() != 1 || !all_conjunctions(rx_inputs)) {
    throw std::runtime_error("inputs to rx should be 1 conjunction");
  }
  auto& rx_input = modules.at(rx_inputs.front());
  rx_inputs = rx_input.inputs;
  if (rx_inputs.size() != 4 || !all_conjunctions(rx_inputs)) {
    throw std::runtime_error("inputs to the input of rx should be 4 conjunctions");
  }

  auto lo_count{0uz};
  auto hi_count{0uz};
  std::array<std::size_t, 4> cycle_lengths;
  cycle_lengths.fill(1);

  struct Signal {
    std::string src, dst;
    bool high;
  };

  for (auto press{1uz}; ranges::any_of(cycle_lengths, [](const auto len) { return len < 2; });
       ++press) {
    const Signal init_signal{
        .src = "button"s,
        .dst = "broadcaster"s,
        .high = false,
    };
    for (std::deque<Signal> q{{init_signal}}; !q.empty(); q.pop_front()) {
      for (const auto& [i, high] : my_std::views::enumerate(rx_input.high | views::values)) {
        if (cycle_lengths[i] < 2 && high) {
          cycle_lengths[i] = press;
        }
      }
      const auto [src, dst, hi] = q.front();
      if (press <= 1000) {
        lo_count += !hi;
        hi_count += hi;
      }

      auto& m{modules.at(dst)};
      switch (m.type) {
        case Module::Type::FlipFlop: {
          if (!hi) {
            m.high[m.id] = !m.high[m.id];
            for (const auto& out : m.outputs) {
              q.push_back({dst, out, m.high[m.id]});
            }
          }
        } break;
        case Module::Type::Broadcaster: {
          for (const auto& out : m.outputs) {
            q.push_back({dst, out, false});
          }
        } break;
        case Module::Type::Conjunction: {
          m.high[src] = hi;
          const auto high_out{!ranges::all_of(m.high | views::values, std::identity{})};
          for (const auto& out : m.outputs) {
            q.push_back({dst, out, high_out});
          }
        } break;
        case Module::Type::Output: {
        } break;
      }
    }
  }

  const auto part1{lo_count * hi_count};
  const auto part2{my_std::ranges::fold_left(
      cycle_lengths,
      1uz,
      [](const auto lcm, const auto len) { return std::lcm(lcm, len); }
  )};

  return std::pair{part1, part2};
}

int main() {
  aoc::init_io();

  const auto modules{update_module_connections(parse_input(std::cin))};
  const auto [part1, part2] = search(modules);

  std::print("{} {}\n", part1, part2);

  return 0;
}
