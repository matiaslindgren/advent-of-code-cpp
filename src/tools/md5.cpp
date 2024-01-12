import std;
import aoc;
import md5;

int main() {
  const auto input{aoc::slurp_file("/dev/stdin")};
  const auto output{md5::sum(input)};
  std::print("{}\n", md5::hexdigest(output));
  return 0;
}
