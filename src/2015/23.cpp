import std;

namespace ranges = std::ranges;
namespace views = std::views;

// TODO(reinterpret 3chars to fixed width type)
enum Code {
  half,
  triple,
  increment,
  jump,
  jump_if_even,
  jump_if_one,
};

std::istream& operator>>(std::istream& is, Code& code) {
  std::string id;
  if (is >> id) {
    if (id == "hlf") {
      code = Code::half;
      return is;
    }
    if (id == "tpl") {
      code = Code::triple;
      return is;
    }
    if (id == "inc") {
      code = Code::increment;
      return is;
    }
    if (id == "jmp") {
      code = Code::jump;
      return is;
    }
    if (id == "jie") {
      code = Code::jump_if_even;
      return is;
    }
    if (id == "jio") {
      code = Code::jump_if_one;
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Code");
}

struct Instruction {
  Code code;
  int addr;
  int jmp;
};

bool isalpha(unsigned char ch) {
  return std::isalpha(ch);
}

std::istream& operator>>(std::istream& is, Instruction& ins) {
  Code code;
  if (is >> code) {
    int jmp{0};
    if (code == Code::jump && is >> jmp) {
      ins = {code, -1, jmp};
      return is;
    }
    std::string addr;
    if (is >> addr && !addr.empty() && isalpha(addr.front())
        && (!(code == Code::jump_if_one || code == Code::jump_if_even) || is >> jmp)) {
      ins = {code, addr.front() - 'a', jmp};
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Instruction");
}

using Memory = std::array<int, 8>;
using Program = std::vector<Instruction>;

void run(Memory& memory, const Program& program) {
  for (int i{0}; 0 <= i && i < program.size();) {
    const Instruction ins = program[i];
    int jump{1};
    switch (ins.code) {
      case Code::half: {
        memory[ins.addr] /= 2;
      } break;
      case Code::triple: {
        memory[ins.addr] *= 3;
      } break;
      case Code::increment: {
        memory[ins.addr] += 1;
      } break;
      case Code::jump: {
        jump = ins.jmp;
      } break;
      case Code::jump_if_even: {
        if (memory[ins.addr] % 2 == 0) {
          jump = ins.jmp;
        }
      } break;
      case Code::jump_if_one: {
        if (memory[ins.addr] == 1) {
          jump = ins.jmp;
        }
      } break;
    }
    i += jump;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);

  Memory memory = {0};
  const auto program{views::istream<Instruction>(std::cin) | ranges::to<Program>()};

  run(memory, program);
  const auto part1{memory[1]};

  memory.fill(0);
  memory[0] = 1;
  run(memory, program);
  const auto part2{memory[1]};

  std::print("{} {}\n", part1, part2);

  return 0;
}
