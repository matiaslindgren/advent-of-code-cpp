#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using aoc::skip;
using std::operator""s;
using Path = std::filesystem::path;
using FileSystem = std::unordered_map<Path, std::unordered_map<Path, long>>;

constexpr auto sum{std::__bind_back(ranges::fold_left, 0L, std::plus{})};

long count_disk_usage(const FileSystem& fs, Path path) {
  if (not fs.contains(path)) {
    return 0;
  }
  return sum(views::transform(fs.at(path), [&fs](auto&& item) {
    auto [subpath, size]{item};
    return size + count_disk_usage(fs, subpath);
  }));
}

auto find_part1(const auto& disk_usage) {
  return sum(views::filter(disk_usage | views::values, [](auto size) { return size <= 100'000; }));
}

auto find_part2(const auto& disk_usage) {
  auto total_usage{disk_usage.at(Path{"/"s})};
  auto max_usage{40'000'000L};
  auto to_remove{total_usage - max_usage};
  return ranges::min(views::filter(disk_usage | views::values, [to_remove](auto usage) {
    return usage >= to_remove;
  }));
}

FileSystem parse_file_system(std::string_view path) {
  FileSystem fs;
  Path cwd;
  const auto lines{aoc::slurp_lines(path)};
  for (auto line : lines) {
    std::istringstream ls{line};
    if (line.at(0) == '$') {
      if (std::string cmd; ls >> skip("$"s) >> cmd) {
        if (cmd == "cd"s) {
          if (std::string dir; ls >> dir) {
            if (dir == ".."s) {
              cwd = cwd.parent_path();
            } else {
              cwd /= dir;
            }
          }
        } else if (cmd == "ls"s) {
        } else {
          ls.setstate(std::ios_base::failbit);
        }
      }
    } else if (int size{}; (line.starts_with("dir"s) and ls >> skip("dir"s)) or ls >> size) {
      if (std::string file; ls >> file) {
        fs[cwd][cwd / file] = size;
      } else {
        ls.setstate(std::ios_base::failbit);
      }
    } else {
      ls.setstate(std::ios_base::failbit);
    }
    if (ls.fail() or not ls.eof()) {
      throw std::runtime_error(std::format("failed parsing line '{}'", line));
    }
  }
  return fs;
}

int main() {
  const FileSystem fs{parse_file_system("/dev/stdin")};
  const auto du{
      fs | views::keys
      | views::transform([&fs](Path p) { return std::pair{p, count_disk_usage(fs, p)}; })
      | ranges::to<std::unordered_map>()
  };

  const auto part1{find_part1(du)};
  const auto part2{find_part2(du)};

  std::print("{} {}\n", part1, part2);

  return 0;
}
