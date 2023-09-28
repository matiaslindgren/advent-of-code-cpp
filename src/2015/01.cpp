import std;

// TODO(llvm17)
namespace my_ranges {
template <class Container>
  requires(!std::ranges::view<Container>)
constexpr auto to() {
  return std::__range_adaptor_closure_t([]<std::ranges::input_range R>(R &&r) {
    Container c;
    std::ranges::copy(r, std::back_inserter(c));
    return c;
  });
}
}  // namespace my_ranges

int main() {
  const auto parse_current_floor = [floor = 0](auto ch) mutable {
    return floor += (ch == '(') - (ch == ')');
  };
  const auto floors = std::views::istream<char>(std::cin) |
                      std::views::transform(parse_current_floor) |
                      // TODO(llvm17)
                      // std::ranges::to<std::vector<int>>();
                      my_ranges::to<std::vector<int>>();

  const auto part1 = floors.back();
  const auto part2 =
      1 +
      std::distance(floors.begin(),
                    std::ranges::find_if(floors, [](auto x) { return x < 0; }));

  std::cout << part1 << ' ' << part2 << '\n';
  return 0;
}
