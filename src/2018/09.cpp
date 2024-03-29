import std;
import aoc;

void left_rotate(auto& q, const auto n) {
  for (int i{}; i < n; ++i) {
    q.push_back(q.front());
    q.pop_front();
  }
}

void right_rotate(auto& q, const auto n) {
  for (int i{}; i < n; ++i) {
    q.push_front(q.back());
    q.pop_back();
  }
}

auto find_winning_score(const auto n_players, const auto max_marble) {
  std::vector<long> scores(n_players, 0);
  std::deque marbles{0L};

  for (int marble{1}; marble <= max_marble; ++marble) {
    if (marble % 23 == 0) {
      right_rotate(marbles, 7);
      scores[marble % scores.size()] += marble + marbles.front();
      marbles.pop_front();
    } else {
      left_rotate(marbles, 2);
      marbles.push_front(marble);
    }
  }

  return std::ranges::max(scores);
}

auto parse_input(std::string path) {
  using aoc::skip;
  using std::operator""s;
  std::istringstream is{aoc::slurp_file(path)};
  if (int n_players; is >> n_players >> std::ws >> skip("players;"s)) {
    if (int max_marble; is >> std::ws >> skip("last marble is worth"s) >> max_marble >> std::ws
                        >> skip("points"s)) {
      return std::pair{n_players, max_marble};
    }
  }
  throw std::runtime_error("failed parsing input state");
}

int main() {
  const auto [n_players, max_marble]{parse_input("/dev/stdin")};

  const auto part1{find_winning_score(n_players, max_marble)};
  const auto part2{find_winning_score(n_players, 100 * max_marble)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
