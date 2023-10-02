import std;

auto look_and_say(const auto& digits) {
  std::vector<int> result;
  for (auto lhs{digits.begin()}; lhs != digits.end();) {
    const auto is_mismatch = [&](auto x) { return x != *lhs; };
    const auto rhs = std::ranges::find_if(lhs, digits.end(), is_mismatch);
    result.push_back(std::ranges::distance(lhs, rhs));
    result.push_back(*lhs);
    lhs = rhs;
  }
  return result;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  auto digits = std::views::istream<char>(std::cin) |
                std::views::transform([](char ch) { return ch - '0'; }) |
                std::ranges::to<std::vector<int>>();

  int iteration{};
  for (; iteration < 40; ++iteration) {
    digits = look_and_say(digits);
  }
  const auto part1{digits.size()};

  for (; iteration < 50; ++iteration) {
    digits = look_and_say(digits);
  }
  const auto part2{digits.size()};

  std::print("{} {}\n", part1, part2);

  return 0;
}
