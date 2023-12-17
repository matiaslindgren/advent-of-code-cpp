import std;
import aoc;
import my_std;

namespace ranges = std::ranges;
namespace views = std::views;

enum class Direction : std::size_t {
  east = 0,
  south = 1,
  west = 2,
  north = 3,
};

struct Blocks {
  static constexpr auto max_loss{std::numeric_limits<int>::max()};
  std::vector<int> loss;
  std::size_t w{};

  constexpr auto width() const {
    return w;
  }
  constexpr auto height() const {
    return loss.size() / width();
  }
  constexpr std::array<std::size_t, 4> adjacent(auto i) const {
    return {i + 1, i + width(), i - 1, i - width()};
  }
};

std::istream& operator>>(std::istream& is, Blocks& blocks) {
  std::vector<int> loss;
  std::size_t width{};
  for (std::string line; std::getline(is, line) && !line.empty();) {
    line = "#" + line + "#";
    if (!width) {
      width = line.size();
    } else if (line.size() != width) {
      is.setstate(std::ios_base::failbit);
      break;
    }
    for (std::stringstream ls{line}; is && ls;) {
      if (char ch; ls >> ch) {
        switch (ch) {
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9': {
            loss.push_back(ch - '0');
          } break;
          case '#': {
            loss.push_back(Blocks::max_loss);
          } break;
          default: {
            is.setstate(std::ios_base::failbit);
          } break;
        }
      }
    }
  }
  if (!loss.empty()) {
    loss.insert_range(loss.begin(), views::repeat(Blocks::max_loss, width));
    loss.insert_range(loss.end(), views::repeat(Blocks::max_loss, width));
    blocks = {.loss = loss, .w = width};
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing blocks");
}

template <typename Int, Int limit = std::numeric_limits<Int>::max()>
  requires std::integral<Int>
constexpr auto saturating_add(Int a, Int b) -> Int {
  return (a > limit - b) ? limit : a + b;
}

constexpr auto search(const auto& blocks, const auto min_moves, const auto max_moves) {
  struct State {
    std::size_t block;
    int moves{};
    Direction direction;
  };

  const auto n_blocks{blocks.loss.size()};
  const auto n_moves{max_moves + 1uz};
  const auto n_directions{4uz};
  std::vector<int> search_space(n_blocks * n_moves * n_directions, Blocks::max_loss);
  const auto loss_data{std::mdspan(search_space.data(), n_blocks, n_moves, n_directions)};

  const auto loss{[&](const auto& state) -> auto& {
    const auto& [block, moves, direction] = state;
    return loss_data[block, moves, std::to_underlying(direction)];
  }};

  const auto push_min_loss_heap{[&](auto& q, const auto& state) {
    q.push_back(state);
    ranges::push_heap(q, ranges::greater{}, loss);
  }};
  const auto pop_min_loss_heap{[&](auto& q) {
    ranges::pop_heap(q, ranges::greater{}, loss);
    const auto s{q.back()};
    q.pop_back();
    return s;
  }};

  const auto begin{blocks.width() + 1};
  const auto end{blocks.width() * blocks.height() - begin - 1};

  std::vector<State> q{
      {begin, 1, Direction::east},
      {begin, 1, Direction::south},
  };
  loss(q[0]) = loss(q[1]) = 0;

  while (!q.empty()) {
    const State src{pop_min_loss_heap(q)};
    if (src.block == end && min_moves <= src.moves && src.moves <= max_moves) {
      return loss(src);
    }
    const auto adjacent{blocks.adjacent(src.block)};
    for (auto [dir, adj_block] : my_std::views::enumerate(adjacent)) {
      if (Direction{((dir + 2) % 4)} == src.direction) {
        continue;
      }
      State dst{
          .block = adj_block,
          .direction = Direction{dir},
      };
      if (dst.direction != src.direction && src.moves < min_moves) {
        continue;
      }
      if (dst.direction == src.direction && src.moves >= max_moves) {
        continue;
      }
      dst.moves = (dst.direction == src.direction ? src.moves + 1 : 1);
      if (const auto new_loss{saturating_add(loss(src), blocks.loss.at(dst.block))};
          new_loss < loss(dst)) {
        loss(dst) = new_loss;
        push_min_loss_heap(q, dst);
      }
    }
  }

  return Blocks::max_loss;
}

int main() {
  aoc::init_io();

  Blocks blocks;
  std::cin >> blocks;

  const auto part1{search(blocks, 1, 3)};
  const auto part2{search(blocks, 4, 10)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
