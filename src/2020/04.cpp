#include "std.hpp"
#include "aoc.hpp"

namespace ranges = std::ranges;
namespace views = std::views;

using std::operator""s;

using Passport = std::unordered_map<std::string, std::string>;
using Passports = std::vector<Passport>;

Passports parse_passports(std::string_view path) {
  Passports ps;
  for (auto&& section_chars : views::split(aoc::slurp_file(path), "\n\n"s)) {
    auto section{std::ranges::to<std::string>(section_chars)};
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
  const std::vector required{
      "byr"s,
      "iyr"s,
      "eyr"s,
      "hgt"s,
      "hcl"s,
      "ecl"s,
      "pid"s,
  };
  return ranges::all_of(required, [&p](auto key) { return p.contains(key); });
}

const std::regex four_digits{"^\\d{4}$"};
const std::regex nine_digits{"^\\d{9}$"};
const std::regex height_cm{"^\\d{2,3}cm$"};
const std::regex height_in{"^\\d{2,3}in$"};
const std::regex hair_color{"^#[0-9a-f]{6}$"};
const std::regex eye_color{"^(amb|blu|brn|gry|grn|hzl|oth)$"};

bool is_valid_entry(const std::string& key, const std::string& val) {
  std::istringstream is{val};
  if (key == "byr"s) {
    if (std::regex_match(val, four_digits)) {
      if (int byr; is >> byr) {
        return 1920 <= byr and byr <= 2002;
      }
    }
  } else if (key == "iyr"s) {
    if (std::regex_match(val, four_digits)) {
      if (int iyr; is >> iyr) {
        return 2010 <= iyr and iyr <= 2020;
      }
    }
  } else if (key == "eyr"s) {
    if (std::regex_match(val, four_digits)) {
      if (int eyr; is >> eyr) {
        return 2020 <= eyr and eyr <= 2030;
      }
    }
  } else if (key == "hgt"s) {
    if (std::regex_match(val, height_cm)) {
      if (int hgt; is >> hgt) {
        return 150 <= hgt and hgt <= 193;
      }
    } else if (std::regex_match(val, height_in)) {
      if (int hgt; is >> hgt) {
        return 59 <= hgt and hgt <= 76;
      }
    }
  } else if (key == "hcl"s) {
    return std::regex_match(val, hair_color);
  } else if (key == "ecl"s) {
    return std::regex_match(val, eye_color);
  } else if (key == "pid"s) {
    return std::regex_match(val, nine_digits);
  } else if (key == "cid"s) {
    return true;
  }
  return false;
}

bool is_valid(const Passport& p) {
  return (is_filled(p) and ranges::all_of(p, [](auto&& kv) {
            return is_valid_entry(kv.first, kv.second);
          }));
}

int main() {
  const Passports ps{parse_passports("/dev/stdin")};
  const auto part1{ranges::count_if(ps, is_filled)};
  const auto part2{ranges::count_if(ps, is_valid)};
  std::print("{} {}\n", part1, part2);
  return 0;
}
