import std;
import aoc;
import my_std;

using std::operator""s;
using std::operator""sv;
using aoc::skip;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Op : int {
  addi = 0,
  addr,
  bani,
  banr,
  bori,
  borr,
  eqir,
  eqri,
  eqrr,
  gtir,
  gtri,
  gtrr,
  muli,
  mulr,
  seti,
  setr,
};
const std::unordered_map<std::string, Op> str2op{
    {"addi"s, Op::addi},
    {"addr"s, Op::addr},
    {"bani"s, Op::bani},
    {"banr"s, Op::banr},
    {"bori"s, Op::bori},
    {"borr"s, Op::borr},
    {"eqir"s, Op::eqir},
    {"eqri"s, Op::eqri},
    {"eqrr"s, Op::eqrr},
    {"gtir"s, Op::gtir},
    {"gtri"s, Op::gtri},
    {"gtrr"s, Op::gtrr},
    {"muli"s, Op::muli},
    {"mulr"s, Op::mulr},
    {"seti"s, Op::seti},
    {"setr"s, Op::setr},
};

struct Instruction {
  Op op;
  long a{}, b{}, c{};
};

std::string fmt_read(auto&& idx) {
  return std::format("r[{}]", idx);
}

std::string elf_instruction_as_cpp(const Instruction& ins) {
  switch (ins.op) {
    case Op::addi:
      return std::format("{} = {} + {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::addr:
      return std::format("{} = {} + {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::bani:
      return std::format("{} = {} & {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::banr:
      return std::format("{} = {} & {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::bori:
      return std::format("{} = {} | {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::borr:
      return std::format("{} = {} | {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::eqir:
      return std::format("{} = {} == {};", fmt_read(ins.c), ins.a, fmt_read(ins.b));
    case Op::eqri:
      return std::format("{} = {} == {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::eqrr:
      return std::format("{} = {} == {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::gtir:
      return std::format("{} = {} > {};", fmt_read(ins.c), ins.a, fmt_read(ins.b));
    case Op::gtri:
      return std::format("{} = {} > {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::gtrr:
      return std::format("{} = {} > {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::muli:
      return std::format("{} = {} * {};", fmt_read(ins.c), fmt_read(ins.a), ins.b);
    case Op::mulr:
      return std::format("{} = {} * {};", fmt_read(ins.c), fmt_read(ins.a), fmt_read(ins.b));
    case Op::seti:
      return std::format("{} = {};", fmt_read(ins.c), ins.a);
    case Op::setr:
      return std::format("{} = {};", fmt_read(ins.c), fmt_read(ins.a));
  }
}

std::ostream& operator<<(std::ostream& os, const Instruction& ins) {
  return os << elf_instruction_as_cpp(ins);
}

std::istream& operator>>(std::istream& is, Op& op) {
  if (std::string s; is >> s) {
    if (str2op.contains(s)) {
      op = str2op.at(s);
    } else {
      is.setstate(std::ios_base::failbit);
    }
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing elf code opcode");
}

std::istream& operator>>(std::istream& is, Instruction& instruction) {
  if (Instruction ins; is >> ins.op >> ins.a >> ins.b >> ins.c) {
    instruction = ins;
  }
  if (is or is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing elf code instruction");
}

auto parse_input(std::string path) {
  std::istringstream is{aoc::slurp_file(path)};
  if (int ip; skip(is, "#ip"s) and is >> ip) {
    const auto instructions{views::istream<Instruction>(is) | ranges::to<std::vector>()};
    if (is or is.eof()) {
      return std::pair{ip, instructions};
    }
  }
  throw std::runtime_error("input is not elf code (aoc year 2018)");
}

void write_lines(std::ostream& os, auto&& lines) {
  ranges::copy(lines, std::ostream_iterator<std::string>(os, "\n"));
}

void generate_code(std::ostream& os, const auto ipr, auto instructions) {
  std::vector lines{
      "using Registers = std::array<long, 6>;"s,
      "auto run(Registers& r) {"s,
      "auto& ip{"s + fmt_read(ipr) + "};"s,
      "constexpr auto n{"s + std::format("{}", instructions.size()) + "};"s,
      "const auto is_running{[&]{return 0 <= ip and ip <= n;}};"s,
      "for (;is_running();++ip){"s,
      "switch (ip){"s,
  };
  for (auto&& [i, ins] : my_std::views::enumerate(instructions)) {
    lines.push_back(std::format("case {}:", i) + "{"s + elf_instruction_as_cpp(ins) + "} break;"s);
  }
  lines.push_back("default: break;"s);
  lines.push_back("}}}"s);
  ranges::copy(lines, std::ostream_iterator<std::string>(os, "\n"));
}

int main() {
  const auto [ipr, instructions]{parse_input("/dev/stdin")};

  std::cout << "/* src/tools/elf2cpp GENERATED CODE BEGIN */\n";
  generate_code(std::cout, ipr, instructions);
  std::cout << "/* src/tools/elf2cpp GENERATED CODE END */\n";

  return 0;
}
