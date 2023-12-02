import std;
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
constexpr auto thread_count{12uz};
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
      const auto msg_len{md5::append_digits(msg, input_size, i)};
      const auto checksum{md5::compute(msg, msg_len)};
      if (checksum & 0xfffff000) {
        continue;
      }
      const auto pw_idx{(checksum & 0xf00) >> 8};
      if (res.pw1.size() < res.password_len) {
        res.pw1 += std::format("{:x}", pw_idx);
      }
      if (pw_idx < res.password_len && !res.pw2[pw_idx]) {
        const auto pw_val{(checksum & 0x0f0) >> 4};
        const auto pw_str{std::format("{:x}", pw_val)};
        res.pw2[pw_idx] = pw_str.front();
      }
    }
    results[i_out] = res;
  }
};

Result parallel_find_passwords(md5::Message msg, const std::size_t input_size) {
  Result res;
  for (auto i{0uz}; i < 100'000'000 && !res.is_complete();
       i += threads.size() * parallel_chunk_size) {
    for (auto t{0uz}; t < threads.size(); ++t) {
      threads[t] = std::thread(
          find_passwords{},
          t,
          msg,
          input_size,
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
      for (auto c{0uz}; c < res.pw2.size(); ++c) {
        auto src{th_res.pw2[c]};
        auto& dst{res.pw2[c]};
        if (src && !dst) {
          dst = src;
        }
      }
    }
  }
  return res;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  md5::Message msg = {0};
  std::size_t input_size{};
  {
    std::string input;
    std::cin >> input;
    input_size = input.size();
    ranges::move(input, msg.begin());
  }

  const auto [part1, part2] = parallel_find_passwords(msg, input_size);
  std::print("{} {}\n", part1, part2);

  return 0;
}
