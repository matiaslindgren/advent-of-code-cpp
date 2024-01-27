import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

struct Step {
  std::string str;
  std::string label;
  int lens;
  enum {
    add,
    rm,
  } command;
};

std::istream& operator>>(std::istream& is, Step& step) {
  if (Step s{}; std::getline(is, s.str, ',') && !s.str.empty()) {
    if (s.str.back() == '\n') {
      s.str.pop_back();
    }
    std::stringstream ls{s.str};
    char ch;
    while (ls.get(ch) && ch != '=' && ch != '-') {
      s.label.push_back(ch);
    }
    if (ch == '=') {
      s.command = Step::add;
      if (ls >> s.lens && s.lens > 0) {
        step = s;
        return is;
      }
    } else if (ch == '-') {
      s.command = Step::rm;
      step = s;
      return is;
    }
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Step");
}

uint8_t hash(std::string_view s) {
  return my_std::ranges::fold_left(s, uint8_t{}, [](const uint8_t h, const char ch) {
    return 17u * (h + ch);
  });
}

constexpr auto sum{std::__bind_back(my_std::ranges::fold_left, 0u, std::plus{})};

auto find_part1(const auto& steps) {
  return sum(steps | views::transform([](auto step) { return hash(step.str); }));
}

// TODO ranges::adjacent
constexpr decltype(auto) window2(ranges::range auto&& r) {
  return views::zip(r, views::drop(r, 1));
}

struct Lens {
  int box;
  int seq_no;
  int lens;
  constexpr auto operator<=>(const Lens&) const = default;
  constexpr auto power(const int slot = 1) const {
    return (box + 1) * slot * lens;
  }
};

auto find_part2(const auto& steps) {
  int seq_no{0};
  std::unordered_map<std::string, Lens> lenses;
  for (const auto& step : steps) {
    switch (step.command) {
      case Step::add: {
        if (const auto it{lenses.find(step.label)}; it != lenses.end()) {
          it->second.lens = step.lens;
        } else {
          lenses[step.label] = {hash(step.label), seq_no++, step.lens};
        }
      } break;
      case Step::rm: {
        lenses.erase(step.label);
      } break;
    }
  }

  auto ordered{lenses | views::values | ranges::to<std::vector>()};
  ranges::sort(ordered);

  return my_std::ranges::fold_left(
      window2(ordered) | views::transform([slot = 1](const auto& w) mutable {
        const auto& [l1, l2] = w;
        if (l1.box != l2.box) {
          slot = 0;
        }
        return l2.power(++slot);
      }),
      ordered.front().power(),
      std::plus{}
  );
}

int main() {
  using std::operator""s;

  std::istringstream input{aoc::slurp_file("/dev/stdin")};
  const auto steps{views::istream<Step>(input) | ranges::to<std::vector>()};

  const auto part1{find_part1(steps)};
  const auto part2{find_part2(steps)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
