#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

struct Reindeer {
  std::string name;
  int speed{};
  int stamina{};
  int rest_need{};
};

struct ReindeerState {
  bool resting{};
  int distance{};
  int fly_time{};
  int rest_time{};
  int points{};
};

auto run_race(const auto& herd) {
  std::vector<ReindeerState> race_state(herd.size());
  for (int t{1}; t <= 2503; ++t) {
    for (auto&& [deer, state] : views::zip(herd, race_state)) {
      if (state.resting) {
        state.rest_time += 1;
        if (state.rest_time >= deer.rest_need) {
          state.rest_time = 0;
          state.resting = false;
        }
      } else {
        state.distance += deer.speed;
        state.fly_time += 1;
        if (state.fly_time >= deer.stamina) {
          state.fly_time = 0;
          state.resting = true;
        }
      }
    }
    ranges::for_each(
        race_state,
        [leader = ranges::max(race_state, {}, &ReindeerState::distance)](auto& state) {
          state.points += int{state.distance == leader.distance};
        }
    );
  }
  return race_state;
}

std::istream& operator>>(std::istream& is, Reindeer& reindeer) {
  using std::operator""s;
  using aoc::skip;
  if (Reindeer r; is >> r.name >> std::ws >> skip("can fly"s) >> r.speed >> std::ws
                  >> skip("km/s for"s) >> r.stamina >> std::ws
                  >> skip("seconds, but then must rest for"s) >> r.rest_need >> std::ws
                  >> skip("seconds."s)) {
    reindeer = r;
  }
  return is;
}

int main() {
  const auto herd{aoc::parse_items<Reindeer>("/dev/stdin")};

  const auto part1{ranges::max(views::transform(run_race(herd), &ReindeerState::distance))};
  const auto part2{ranges::max(views::transform(run_race(herd), &ReindeerState::points))};

  std::println("{} {}", part1, part2);

  return 0;
}
