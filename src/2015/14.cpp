import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""sv;

struct Reindeer {
  std::string name;
  int speed;
  int stamina;
  int rest_need;
};

std::istream& operator>>(std::istream& is, Reindeer& r) {
  using std::operator""s;
  using aoc::skip;
  if (is >> r.name >> skip(" can fly"s) >> r.speed >> skip(" km/s for"s) >> r.stamina
      >> skip(" seconds, but then must rest for"s) >> r.rest_need >> skip(" seconds."s)) {
    return is;
  }
  if (is.eof()) {
    return is;
  }
  throw std::runtime_error("failed parsing Reindeer");
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  const auto herd{views::istream<Reindeer>(input) | ranges::to<std::vector>()};

  struct ReindeerState {
    bool resting;
    int distance;
    int fly_time;
    int rest_time;
    int points;
  };
  std::vector<ReindeerState> race_state(herd.size());
  const auto find_leader{[&race_state](const auto& state_accessor) {
    return *ranges::max_element(race_state, {}, state_accessor);
  }};

  for (int t{1}; t <= 2503; ++t) {
    for (const auto& p : views::zip(herd, race_state)) {
      const auto& [r, s] = p;
      if (s.resting) {
        s.rest_time += 1;
        if (s.rest_time >= r.rest_need) {
          s.rest_time = 0;
          s.resting = false;
        }
      } else {
        s.distance += r.speed;
        s.fly_time += 1;
        if (s.fly_time >= r.stamina) {
          s.fly_time = 0;
          s.resting = true;
        }
      }
    }
    const auto leader_state{find_leader(&ReindeerState::distance)};
    for (auto& s : race_state) {
      s.points += (s.distance == leader_state.distance);
    }
  }

  const auto part1{find_leader(&ReindeerState::distance).distance};
  const auto part2{find_leader(&ReindeerState::points).points};

  std::print("{} {}\n", part1, part2);

  return 0;
}
