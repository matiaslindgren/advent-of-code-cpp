import std;
import md5;

int main() {
  md5::Message input;
  for (char ch; std::cin.get(ch);) {
    input.push_back(ch);
  }
  std::print("{}\n", md5::hexdigest(md5::compute(input)));
  return 0;
}
