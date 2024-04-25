#include "md5.hpp"

#include "aoc.hpp"
#include "std.hpp"

int main() {
  const auto input{aoc::slurp_file("/dev/stdin")};
  const auto output{md5::sum(input)};
  std::print("{}\n", md5::hexdigest(output));
  return 0;
}
