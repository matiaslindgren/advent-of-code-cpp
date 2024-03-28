import std;
import aoc;
import intcode;

using std::operator""s;

using aoc::skip;
using intcode::IntCode;
using Vec2 = aoc::Vec2<int>;

enum class ParseState {
  lines,
  command,
  doors,
  items,
  invalid,
};

using Strings = std::unordered_set<std::string>;

struct Options {
  Strings doors, items;
  bool invalid{false};
  long password{};

  bool empty() const {
    return doors.empty() and items.empty();
  }

  void clear() {
    doors.clear();
    items.clear();
  }

  bool operator==(const Options& rhs) const = default;
};

std::string get_line(IntCode& ic) {
  std::ostringstream os;
  while (not ic.is_done()) {
    ic.do_step();
    if (auto out{ic.pop_output()}) {
      if (auto val{out.value()}; 0 < val and val < 128) {
        if (auto ch{static_cast<char>(val)}; ch == '\n') {
          break;
        } else {
          os << ch;
        }
      }
    }
  }
  return os.str();
}

Options run_until_prompt(IntCode& ic) {
  Options opts;
  int n_empty{};

  for (auto s{ParseState::lines};;) {
    if (n_empty > 5) {
      s = ParseState::invalid;
    }

    switch (s) {
      case ParseState::lines: {
        auto line{get_line(ic)};
        if (line == "Command?"s) {
          s = ParseState::command;
        } else if (line == "Items here:"s) {
          s = ParseState::items;
        } else if (line == "Doors here lead:"s) {
          s = ParseState::doors;
        } else if (line.starts_with("\"Oh, hello!"s)) {
          std::istringstream is{line};
          if (long pass;
              is >> skip("\"Oh, hello! You should be able to get in by typing"s) >> pass) {
            return {.password = pass};
          } else {
            throw std::runtime_error(
                std::format("got into room but couldn't parse password from '{}'", line)
            );
          }
        } else if (line.ends_with("you are ejected back to the checkpoint."s)) {
          s = ParseState::invalid;
        } else if (line.ends_with("infinite loop."s)) {
          s = ParseState::invalid;
        }
        n_empty = line.empty() ? n_empty + 1 : 0;
      } break;

      case ParseState::doors: {
        auto door{get_line(ic)};
        if (door.starts_with("- "s)) {
          opts.doors.insert(door.substr(2));
        } else {
          s = ParseState::lines;
        }
      } break;

      case ParseState::items: {
        auto item{get_line(ic)};
        if (item.starts_with("- "s)) {
          opts.items.insert(item.substr(2));
        } else {
          s = ParseState::lines;
        }
      } break;

      case ParseState::command: {
        return opts;
      } break;

      case ParseState::invalid: {
        return {.invalid = true};
      } break;
    }
  }
}

struct SearchState {
  Vec2 pos;
  Strings inventory;
  std::string cmd;
  IntCode ic;
  Options opts;
};

auto explore(const auto& program) {
  std::unordered_map<Vec2, std::vector<Strings>> seen;

  for (std::deque q{SearchState{.ic = IntCode(program)}}; not q.empty(); q.pop_front()) {
    auto s{q.front()};

    if (s.inventory.size() > 4) {
      continue;
    }

    if (auto it{seen.find(s.pos)}; it != seen.end()) {
      if (std::ranges::find(it->second, s.inventory) != it->second.end()) {
        continue;
      }
    }
    seen[s.pos].push_back(s.inventory);

    if (not s.cmd.empty()) {
      s.ic.input.append_range(s.cmd + "\n"s);
    }

    auto opts{run_until_prompt(s.ic)};

    if (not s.cmd.starts_with("drop"s) and not s.cmd.starts_with("take"s)) {
      s.opts = opts;
    }
    if (s.opts.password) {
      return s.opts.password;
    }
    if (s.opts.invalid) {
      continue;
    }

    for (auto item : s.inventory) {
      if (s.cmd != std::format("take {}", item)) {
        q.push_back(s);
        q.back().inventory.erase(item);
        q.back().cmd = std::format("drop {}", item);
      }
    }

    for (const auto& door : s.opts.doors) {
      q.push_back(s);
      if (auto& pos{q.back().pos}; door == "north"s) {
        pos.y() -= 1;
      } else if (door == "south"s) {
        pos.y() += 1;
      } else if (door == "west"s) {
        pos.x() -= 1;
      } else if (door == "east"s) {
        pos.x() += 1;
      }
      q.back().opts.clear();
      q.back().cmd = std::format("{}", door);
    }

    for (const auto& item : s.opts.items) {
      if (not s.inventory.contains(item)) {
        q.push_back(s);
        q.back().inventory.insert(item);
        q.back().opts.items.erase(item);
        q.back().cmd = std::format("take {}", item);
      }
    }
  }

  throw std::runtime_error("could not find target");
}

int main() {
  const auto program{intcode::parse_program(aoc::slurp_file("/dev/stdin"))};
  const auto part1{explore(program)};
  std::print("{}\n", part1);
  return 0;
}
