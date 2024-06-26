#include "aoc.hpp"
#include "std.hpp"

using aoc::skip;
using std::operator""s;

namespace ranges = std::ranges;
namespace views = std::views;

struct Instruction {
  int op_id{};
  int a{};
  int b{};
  int c{};
};

using Memory = std::array<int, 4>;

struct Sample {
  Memory before{};
  Memory after{};
  Instruction ins;
};

const std::array opcodes{
    "addi"s,
    "addr"s,
    "bani"s,
    "banr"s,
    "bori"s,
    "borr"s,
    "eqir"s,
    "eqri"s,
    "eqrr"s,
    "gtir"s,
    "gtri"s,
    "gtrr"s,
    "muli"s,
    "mulr"s,
    "seti"s,
    "setr"s,
};

inline constexpr auto update{
    [](auto& mem, const Instruction& ins, const bool immediate, auto&& binop) {
      mem.at(ins.c) = binop(mem.at(ins.a), immediate ? ins.b : mem.at(ins.b));
    }
};

auto compute(Memory mem, const Instruction& ins, const auto& op) {
  if (op == "setr"s) {
    mem.at(ins.c) = mem.at(ins.a);
  } else if (op == "seti"s) {
    mem.at(ins.c) = ins.a;
  } else if (op.starts_with("add"s)) {
    update(mem, ins, op[3] == 'i', std::plus{});
  } else if (op.starts_with("mul"s)) {
    update(mem, ins, op[3] == 'i', std::multiplies{});
  } else if (op.starts_with("ban"s)) {
    update(mem, ins, op[3] == 'i', std::bit_and{});
  } else if (op.starts_with("bor"s)) {
    update(mem, ins, op[3] == 'i', std::bit_or{});
  } else if (op == "gtir"s) {
    update(mem, Instruction{.a = ins.b, .b = ins.a, .c = ins.c}, true, std::less{});
  } else if (op.starts_with("gtr"s)) {
    update(mem, ins, op[3] == 'i', std::greater{});
  } else if (op == "eqir"s) {
    update(mem, Instruction{.a = ins.b, .b = ins.a, .c = ins.c}, true, std::equal_to{});
  } else if (op.starts_with("eqr"s)) {
    update(mem, ins, op[3] == 'i', std::equal_to{});
  }
  return mem;
}

auto analyse_sample(const Sample& sample) {
  return views::filter(
             opcodes,
             [&sample](auto&& code) {
               const auto out{compute(sample.before, sample.ins, code)};
               return ranges::equal(out, sample.after);
             }
         )
         | ranges::to<std::vector>();
}

auto find_part1(const auto& samples) {
  return ranges::count_if(samples, [](auto&& s) { return analyse_sample(s).size() >= 3; });
}

constexpr auto is_empty{[](auto&& c) { return c.empty(); }};

auto resolve_samples(const auto& samples) {
  std::vector<std::vector<std::string>> possible(
      opcodes.size(),
      opcodes | ranges::to<std::vector>()
  );
  for (const auto& s : samples) {
    const auto prev{std::exchange(possible[s.ins.op_id], {})};
    ranges::set_intersection(prev, analyse_sample(s), std::back_inserter(possible[s.ins.op_id]));
  }

  std::vector<std::string> confirmed(opcodes.size());
  while (not ranges::all_of(possible, is_empty)) {
    const auto it{ranges::find_if(possible, [](auto&& p) { return p.size() == 1; })};
    if (it == possible.end()) {
      throw std::runtime_error("there must be at least 1 unambiguous opcode at all times");
    }
    const auto op{it->front()};
    confirmed[it - possible.begin()] = op;
    for (auto& p : possible) {
      const auto rm{ranges::remove(p, op)};
      p.erase(rm.begin(), rm.end());
    }
  }

  if (ranges::any_of(confirmed, is_empty)) {
    throw std::runtime_error("all ambiguous opcodes must be resolved");
  }
  return confirmed;
}

auto find_part2(const auto& samples, const auto& instructions) {
  const auto id2op{resolve_samples(samples)};
  Memory mem;
  mem.fill(0);
  for (const auto& ins : instructions) {
    mem = compute(mem, ins, id2op[ins.op_id]);
  }
  return mem.front();
}

std::istream& operator>>(std::istream& is, Memory& memory) {
  if (Memory m{}; is >> std::ws >> skip("["s)) {
    for (auto i{0UZ}; i < m.size() - 1 and is >> m.at(i) >> skip(","s);) {
      ++i;
    }
    if (is >> m.back() >> skip("]"s)) {
      memory = m;
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Instruction& instruction) {
  if (Instruction ins; is >> ins.op_id >> ins.a >> ins.b >> ins.c) {
    instruction = ins;
  }
  return is;
}

std::istream& operator>>(std::istream& is, Sample& sample) {
  if (Memory before; is >> std::ws >> skip("Before:"s) >> before) {
    if (Instruction ins; is >> ins) {
      if (Memory after; is >> std::ws >> skip("After:"s) >> after) {
        sample = {before, after, ins};
      }
    }
  }
  return is;
}

auto parse_input(std::string path) {
  const auto input{aoc::slurp_file(path)};
  const auto sections{
      views::split(input, "\n\n\n"s)
      | views::transform([](auto&& r) { return ranges::to<std::string>(r); })
      | ranges::to<std::vector<std::string>>()
  };
  if (sections.size() != 2) {
    throw std::runtime_error("input must contain 2 sections separated by 3 newlines");
  }

  std::istringstream is{sections[0]};
  const auto samples{views::istream<Sample>(is) | ranges::to<std::vector>()};
  if (is or is.eof()) {
    is = std::istringstream(sections[1]);
    const auto instructions{views::istream<Instruction>(is) | ranges::to<std::vector>()};
    if (is or is.eof()) {
      return std::pair{samples, instructions};
    }
  }
  throw std::runtime_error("failed parsing input");
}

int main() {
  const auto [samples, instructions]{parse_input("/dev/stdin")};

  const auto part1{find_part1(samples)};
  const auto part2{find_part2(samples, instructions)};

  std::println("{} {}", part1, part2);

  return 0;
}
