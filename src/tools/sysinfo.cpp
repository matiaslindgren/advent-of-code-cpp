import std;
import aoc;

template <typename T>
void print_typeinfo(const auto name) {
  const auto digits{std::numeric_limits<T>::digits};
  const auto min{std::numeric_limits<T>::min()};
  const auto max{std::numeric_limits<T>::max()};
  constexpr auto fmt{
      std::is_floating_point<T>() ? "{:>20} size {} digits {:>2} min {:g} max {:g}\n"
                                  : "{:>20} size {} digits {:>2} min {:d} max {:d}\n"
  };
  std::print(fmt, name, sizeof(T), digits, min, max);
}

int main() {
  std::ios::sync_with_stdio(false);

  std::print("{} cpus\n", aoc::cpu_count());
  print_typeinfo<bool>("bool");
  print_typeinfo<char>("char");
  print_typeinfo<unsigned char>("unsigned char");
  print_typeinfo<int>("int");
  print_typeinfo<unsigned>("unsigned");
  print_typeinfo<long>("long");
  print_typeinfo<long long>("long long");
  print_typeinfo<unsigned long>("unsigned long");
  print_typeinfo<unsigned long long>("unsigned long long");
  print_typeinfo<float>("float");
  print_typeinfo<double>("double");
  print_typeinfo<long double>("long double");

  return 0;
}
