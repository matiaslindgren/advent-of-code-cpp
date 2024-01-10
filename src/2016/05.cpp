import std;
import aoc;
import my_std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

struct Result {
  static constexpr auto password_len{8uz};
  std::string pw1;
  std::string pw2;

  explicit Result() : pw1{}, pw2(password_len, 0) {
  }

  constexpr bool is_complete() const {
    return pw1.size() == password_len && pw2.find('\0') == std::string::npos;
  }
};

constexpr auto parallel_chunk_size{1uz << 18};
const auto thread_count{aoc::cpu_count()};
std::vector<Result> results(thread_count);
std::vector<std::thread> threads(thread_count);

struct find_passwords {
  void operator()(
      const auto i_out,
      md5::Message msg,
      const auto input_size,
      const auto begin,
      const auto count
  ) const {
    Result res;
    for (auto i{begin}; i < begin + count && !res.is_complete(); ++i) {
      msg.len = input_size;
      md5::append_digits(msg, i);
      const auto checksum{md5::compute(msg)};
      if (ranges::any_of(checksum | views::take(5), std::identity{})) {
        continue;
      }
      const auto pw_idx{checksum[5]};
      if (res.pw1.size() < res.password_len) {
        res.pw1 += std::format("{:x}", pw_idx);
      }
      if (pw_idx < res.password_len && !res.pw2[pw_idx]) {
        const auto pw_val{checksum[6]};
        const auto pw_str{std::format("{:x}", pw_val)};
        res.pw2[pw_idx] = pw_str.front();
      }
    }
    results[i_out] = res;
  }
};

Result parallel_find_passwords(md5::Message msg) {
  Result res;
  for (auto i{0uz}; i < 100'000'000 && !res.is_complete();
       i += threads.size() * parallel_chunk_size) {
    for (auto&& [t, th] : my_std::views::enumerate(threads)) {
      th = std::thread(
          find_passwords{},
          t,
          msg,
          msg.len,
          i + t * parallel_chunk_size,
          parallel_chunk_size
      );
    }
    for (auto& th : threads) {
      th.join();
    }
    for (const auto& th_res : results) {
      for (auto ch : th_res.pw1) {
        if (res.pw1.size() < res.password_len) {
          res.pw1.push_back(ch);
        }
      }
      for (auto&& [pos, dst] : my_std::views::enumerate(res.pw2)) {
        auto src{th_res.pw2[pos]};
        if (src && !dst) {
          dst = src;
        }
      }
    }
  }
  return res;
}

int main() {
  aoc::init_io();

  md5::Message msg;
  std::cin >> msg;

  const auto [part1, part2] = parallel_find_passwords(msg);
  std::print("{} {}\n", part1, part2);

  return 0;
}
