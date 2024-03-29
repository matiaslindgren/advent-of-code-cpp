import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr auto min{
    std::__bind_back(my_std::ranges::fold_left, std::numeric_limits<unsigned>::max(), ranges::min)
};
constexpr auto max{
    std::__bind_back(my_std::ranges::fold_left, std::numeric_limits<unsigned>::min(), ranges::max)
};

auto search(const auto& ids) {
  const auto min_id{min(ids)};
  const auto max_id{max(ids)};
  auto part2{min_id};
  for (auto id{min_id}; id <= max_id; ++id) {
    if (not ids.contains(id)) {
      part2 = id;
    }
  }
  return std::pair{max_id, part2};
}

auto parse_input(std::string_view path) {
  std::unordered_set<unsigned> ids;
  {
    std::istringstream is{aoc::slurp_file(path)};
    for (std::string line; std::getline(is, line) and not line.empty();) {
      unsigned id{};
      for (auto [i, ch] : my_std::views::enumerate(line, 1)) {
        if (ch == 'B' or ch == 'R') {
          id += 1 << (line.size() - i);
        }
      }
      ids.insert(id);
    }
    if (not is and not is.eof()) {
      throw std::runtime_error("input is invalid");
    }
  }
  if (ids.empty()) {
    throw std::runtime_error("input is empty");
  }
  return ids;
}

int main() {
  const auto [part1, part2]{search(parse_input("/dev/stdin"))};
  std::print("{} {}\n", part1, part2);
  return 0;
}
