import std;

auto run() {
  long r5_0{-1};
  long r5_1{-1};
  std::array<long, 6> r;
  r.fill(0);
  for (std::unordered_set<long> seen;; r[1] += 1) {
    switch (r[1]) {
      case 0: {
        r[5] = 123;
      } break;
      case 1: {
        r[5] = r[5] & 456;
      } break;
      case 2: {
        r[5] = r[5] == 72;
      } break;
      case 3: {
        r[1] = r[5] + r[1];
      } break;
      case 4: {
        r[1] = 0;
      } break;
      case 5: {
        r[5] = 0;
      } break;
      case 6: {
        r[4] = r[5] | 65536;
      } break;
      case 7: {
        r[5] = 13159625;
      } break;
      case 8: {
        r[3] = r[4] & 255;
      } break;
      case 9: {
        r[5] = r[5] + r[3];
      } break;
      case 10: {
        r[5] = r[5] & 16777215;
      } break;
      case 11: {
        r[5] = r[5] * 65899;
      } break;
      case 12: {
        r[5] = r[5] & 16777215;
      } break;
      case 13: {
        r[3] = 256 > r[4];
      } break;
      case 14: {
        r[1] = r[3] + r[1];
      } break;
      case 15: {
        r[1] = r[1] + 1;
      } break;
      case 16: {
        r[1] = 27;
      } break;
      case 17: {
        r[3] = 0;
      } break;
      case 18: {
        r[2] = r[3] + 1;
      } break;
      case 19: {
        r[2] = r[2] * 256;
      } break;
      case 20: {
        r[2] = r[2] > r[4];
      } break;
      case 21: {
        r[1] = r[2] + r[1];
      } break;
      case 22: {
        r[1] = r[1] + 1;
      } break;
      case 23: {
        r[1] = 25;
      } break;
      case 24: {
        r[3] = r[3] + 1;
      } break;
      case 25: {
        r[1] = 17;
      } break;
      case 26: {
        r[4] = r[3];
      } break;
      case 27: {
        r[1] = 7;
      } break;
      case 28: {
        r[3] = r[5] == r[0];
        if (seen.contains(r[5])) {
          return std::pair{r5_0, r5_1};
        }
        seen.insert(r[5]);
        if (r5_0 < 0) {
          r5_0 = r[5];
        }
        r5_1 = r[5];
      } break;
      case 29: {
        r[1] = r[3] + r[1];
      } break;
      case 30: {
        r[1] = 5;
      } break;
      default:
        break;
    }
  }
}

int main() {
  const auto [part1, part2]{run()};
  std::cout << part1 << " " << part2 << "\n";
  return 0;
}
