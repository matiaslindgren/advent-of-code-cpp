import std;
import aoc;

namespace ranges = std::ranges;
namespace views = std::views;

constexpr static auto alphabet_size{'z' - 'a' + 1};

constexpr int char2digit(char ch) {
  return ch - 'a';
}
constexpr char digit2char(int x) {
  return x + 'a';
}

constexpr static auto forbidden_chars{views::transform("iol", char2digit)};

bool valid(const auto& password) {
  const auto begin{password.begin()};
  const auto end{password.end()};

  if (ranges::find_first_of(password, forbidden_chars) != end) {
    return false;
  }

  {
    std::bitset<alphabet_size> pairs;
    for (auto it{begin}; it != end; ranges::advance(it, 2, end)) {
      it = ranges::adjacent_find(it, end);
      if (it != end) {
        pairs[*it] = true;
      }
    }
    if (pairs.count() < 2) {
      return false;
    }
  }

  // TODO(llvm18)
  // std::views::adjacent<3>(password) ...
  return ranges::any_of(
      views::zip(password, views::drop(password, 1), views::drop(password, 2)),
      [](const auto& t) {
        const auto& [x0, x1, x2] = t;
        return x0 + 1 == x1 and x1 + 1 == x2;
      }
  );
}

template <auto base>
auto increment(const auto& input) {
  std::vector<int> result(input.size());
  int carry{1};
  for (const auto& t : views::reverse(views::zip(result, input))) {
    auto& [dst, src] = t;
    dst = src + carry;
    carry = dst == base;
    dst %= base;
  }
  return result;
}

auto to_string(const auto& password) {
  return views::transform(password, digit2char) | ranges::to<std::string>();
}

int main() {
  std::istringstream input{aoc::slurp_file("/dev/stdin")};

  auto password{
      views::istream<char>(input) | views::transform(char2digit) | ranges::to<std::vector>()
  };

  std::string parts[2];
  for (auto& part : parts) {
    do {
      password = increment<alphabet_size>(password);
    } while (not valid(password));
    part = to_string(password);
  }

  std::print("{} {}\n", parts[0], parts[1]);

  return 0;
}
