#include "aoc.hpp"
#include "std.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

using Passport = std::unordered_map<std::string, std::string>;
using Passports = std::vector<Passport>;

Passports parse_passports(std::string_view path) {
  Passports ps;
  for (auto&& section_chars : views::split(aoc::slurp_file(path), "\n\n"s)) {
    auto section{ranges::to<std::string>(section_chars)};
    ranges::replace(section, ':', ' ');
    std::istringstream is{section};
    ps.emplace_back();
    for (std::string key, val; is >> key >> val;) {
      ps.back()[key] = val;
    }
    if (not is.eof()) {
      throw std::runtime_error(std::format("failed parsing section '{}'", section));
    }
  }
  return ps;
}

bool is_filled(const Passport& p) {
  return ranges::all_of(
      std::initializer_list<std::string>{"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"},
      [&p](auto key) { return p.contains(key); }
  );
}

const std::regex four_digits{"^\\d{4}$"};
const std::regex nine_digits{"^\\d{9}$"};
const std::regex height_cm{"^\\d{2,3}cm$"};
const std::regex height_in{"^\\d{2,3}in$"};
const std::regex hair_color{"^#[0-9a-f]{6}$"};
const std::regex eye_color{"^(amb|blu|brn|gry|grn|hzl|oth)$"};

bool between(int x, int lo, int hi) {
  return lo <= x and x <= hi;
}

bool is_valid_entry(const std::string& key, const std::string& val) {
  std::istringstream is{val};
  if (int byr{}; key == "byr"s and std::regex_match(val, four_digits) and is >> byr) {
    return between(byr, 1920, 2002);
  }
  if (int iyr{}; key == "iyr"s and std::regex_match(val, four_digits) and is >> iyr) {
    return between(iyr, 2010, 2020);
  }
  if (int eyr{}; key == "eyr"s and std::regex_match(val, four_digits) and is >> eyr) {
    return between(eyr, 2020, 2030);
  }
  if (int hgt{}; key == "hgt"s and std::regex_match(val, height_cm) and is >> hgt) {
    return between(hgt, 150, 193);
  }
  if (int hgt{}; key == "hgt"s and std::regex_match(val, height_in) and is >> hgt) {
    return between(hgt, 59, 76);
  }
  if (key == "hcl"s) {
    return std::regex_match(val, hair_color);
  }
  if (key == "ecl"s) {
    return std::regex_match(val, eye_color);
  }
  if (key == "pid"s) {
    return std::regex_match(val, nine_digits);
  }
  if (key == "cid"s) {
    return true;
  }
  return false;
}

bool is_valid(const Passport& p) {
  return is_filled(p)
         and ranges::all_of(p, [](auto&& kv) { return is_valid_entry(kv.first, kv.second); });
}

int main() {
  const Passports ps{parse_passports("/dev/stdin")};
  const auto part1{ranges::count_if(ps, is_filled)};
  const auto part2{ranges::count_if(ps, is_valid)};
  std::println("{} {}", part1, part2);
  return 0;
}
