#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

using Strings = std::vector<std::string>;

struct Module {
  enum class Type : unsigned char {
    Broadcaster,
    FlipFlop,
    Conjunction,
    Output,
  } type{};
  std::string id;
  Strings inputs;
  Strings outputs;
  std::unordered_map<std::string, bool> high;

  [[nodiscard]]
  auto compute_step(const std::string& src, bool hi) {
    bool send_out{false};
    bool high_out{false};
    switch (type) {
      case Type::FlipFlop: {
        if (not hi) {
          send_out = true;
          high_out = (high[id] = not high[id]);
        }
      } break;
      case Type::Broadcaster: {
        send_out = true;
        high_out = false;
      } break;
      case Type::Conjunction: {
        send_out = true;
        high[src] = hi;
        high_out = not ranges::all_of(high | views::values, std::identity{});
      } break;
      case Type::Output: {
      } break;
    }
    return std::pair{send_out, high_out};
  }
};

using Modules = std::unordered_map<std::string, Module>;

auto update_module_connections(auto modules) {
  std::unordered_map<std::string, Strings> all_inputs;
  for (const auto& [id, m] : modules) {
    for (const auto& o : m.outputs) {
      all_inputs[o].push_back(id);
    }
  }
  for (const auto& [id, inputs] : all_inputs) {
    if (not modules.contains(id)) {
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

Module& find_rx_input(auto& modules) {
  const auto all_conjunctions{[&modules](const auto& ms) {
    return not ms.empty() and ranges::all_of(ms, [&modules](const auto& m) {
      return modules.at(m).type == Module::Type::Conjunction;
    });
  }};
  Strings rx_inputs{modules.at("rx"s).inputs};
  if (rx_inputs.size() != 1 or not all_conjunctions(rx_inputs)) {
    throw std::runtime_error("inputs to rx should be 1 conjunction");
  }
  auto& rx_input = modules.at(rx_inputs.front());
  rx_inputs = rx_input.inputs;
  if (rx_inputs.size() != 4 or not all_conjunctions(rx_inputs)) {
    throw std::runtime_error("inputs to the input of rx should be 4 conjunctions");
  }
  return rx_input;
}

bool all_cycles_found(const auto& cycle_lengths) {
  return ranges::all_of(cycle_lengths, [](const auto len) { return len > 1; });
}

struct Signal {
  std::string src;
  std::string dst;
  bool high{false};
};

auto search(auto modules) {
  auto lo_count{0UL};
  auto hi_count{0UL};
  std::array cycle_lengths{1UL, 1UL, 1UL, 1UL};

  auto& rx_input{find_rx_input(modules)};

  for (auto press{1UL}; not all_cycles_found(cycle_lengths); press += 1) {
    const Signal init_signal{
        .src = "button"s,
        .dst = "broadcaster"s,
        .high = false,
    };
    for (std::deque<Signal> q{{init_signal}}; not q.empty(); q.pop_front()) {
      for (auto&& [cycle_len, high] : views::zip(cycle_lengths, rx_input.high | views::values)) {
        if (high and cycle_len < 2) {
          cycle_len = press;
        }
      }
      const auto [src, dst, hi]{q.front()};
      if (press <= 1000) {
        lo_count += not hi;
        hi_count += hi;
      }
      Module& m{modules.at(dst)};
      if (auto [send_out, high_out]{m.compute_step(src, hi)}; send_out) {
        for (const auto& out : m.outputs) {
          q.push_back({
              .src = dst,
              .dst = out,
              .high = high_out,
          });
        }
      }
    }
  }

  auto part1{lo_count * hi_count};
  auto part2{ranges::fold_left(cycle_lengths, 1UL, [](auto lcm, auto len) {
    return std::lcm(lcm, len);
  })};

  return std::pair{part1, part2};
}

std::istream& operator>>(std::istream& is, Module::Type& t) {
  using Type = Module::Type;
  auto ch{is.peek()};
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
    std::istringstream ls{line};
    if (Module::Type type{}; ls >> type) {
      if (std::string id; ls >> id and not id.empty()) {
        if (ls >> std::ws >> skip("->"s)) {
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

auto parse_input(std::string_view path) {
  std::istringstream is{aoc::slurp_file(path)};
  return views::istream<Module>(is) | views::transform([](auto&& m) { return std::pair{m.id, m}; })
         | ranges::to<Modules>();
}

int main() {
  const auto modules{update_module_connections(parse_input("/dev/stdin"))};
  const auto [part1, part2] = search(modules);
  std::println("{} {}", part1, part2);
  return 0;
}
