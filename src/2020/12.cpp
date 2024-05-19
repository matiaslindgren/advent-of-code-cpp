#include "aoc.hpp"
#include "ndvec.hpp"
#include "std.hpp"

using Vec2 = ndvec::vec2<double>;

struct Action {
  enum class Type : char {
    north = 'N',
    south = 'S',
    east = 'E',
    west = 'W',
    left = 'L',
    right = 'R',
    forward = 'F',
  } type;
  int value;
};

std::istream& operator>>(std::istream& is, Action::Type& type) {
  using Type = Action::Type;

  if (std::underlying_type_t<Type> ch; is >> ch) {
    switch (ch) {
      case std::to_underlying(Type::north):
      case std::to_underlying(Type::south):
      case std::to_underlying(Type::east):
      case std::to_underlying(Type::west):
      case std::to_underlying(Type::left):
      case std::to_underlying(Type::right):
      case std::to_underlying(Type::forward): {
        type = {ch};
      } break;
      default: {
        throw std::runtime_error(std::format("unknown action type {}", ch));
      }
    }
  }
  return is;
}

std::istream& operator>>(std::istream& is, Action& action) {
  if (Action::Type type; is >> type) {
    if (int value; is >> value) {
      action = {type, value};
    }
  }
  return is;
}

auto deg2rad(int deg) {
  return deg * std::numbers::pi_v<double> / 180.0;
}

auto rotate(const Vec2& p, int deg) {
  const auto cos{std::round(std::cos(deg2rad(deg)))};
  const auto sin{std::round(std::sin(deg2rad(deg)))};
  return Vec2(p.x() * cos - p.y() * sin, p.x() * sin + p.y() * cos);
}

auto find_part1(const auto& actions) {
  using Type = Action::Type;

  Vec2 pos, dir(1, 0);
  for (const Action& a : actions) {
    switch (a.type) {
      case Type::north: {
        pos.y() += a.value;
      } break;
      case Type::south: {
        pos.y() -= a.value;
      } break;
      case Type::east: {
        pos.x() += a.value;
      } break;
      case Type::west: {
        pos.x() -= a.value;
      } break;
      case Type::left: {
        dir = rotate(dir, a.value);
      } break;
      case Type::right: {
        dir = rotate(dir, -a.value);
      } break;
      case Type::forward: {
        pos.x() += dir.x() * a.value;
        pos.y() += dir.y() * a.value;
      } break;
    }
  }
  return pos.distance(Vec2());
}

auto find_part2(const auto& actions) {
  using Type = Action::Type;

  Vec2 pos, wpos(10, 1);
  for (const Action& a : actions) {
    switch (a.type) {
      case Type::north: {
        wpos.y() += a.value;
      } break;
      case Type::south: {
        wpos.y() -= a.value;
      } break;
      case Type::east: {
        wpos.x() += a.value;
      } break;
      case Type::west: {
        wpos.x() -= a.value;
      } break;
      case Type::left: {
        wpos = rotate(wpos, a.value);
      } break;
      case Type::right: {
        wpos = rotate(wpos, -a.value);
      } break;
      case Type::forward: {
        pos.x() += wpos.x() * a.value;
        pos.y() += wpos.y() * a.value;
      } break;
    }
  }
  return pos.distance(Vec2());
}

int main() {
  const auto actions{aoc::parse_items<Action>("/dev/stdin")};

  const auto part1{find_part1(actions)};
  const auto part2{find_part2(actions)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
