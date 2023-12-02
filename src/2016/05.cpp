import std;
import md5;

namespace ranges = std::ranges;
namespace views = std::views;

inline std::size_t append_digits(md5::Message& msg, const auto msg_size,
                                 const auto number) {
  std::array<uint8_t, 16> digits = {0};
  std::size_t digit_count{0};
  for (auto x{number}; x; x /= 10) {
    digits[digit_count++] = '0' + (x % 10);
  }
  ranges::move(digits | views::take(digit_count) | views::reverse,
               msg.begin() + msg_size);
  return msg_size + digit_count;
}

struct Result {
  static constexpr auto password_len{8uz};
  std::string pw1;
  std::string pw2;

  explicit Result() : pw1{}, pw2(password_len, 0) {}

  constexpr bool is_complete() const {
    return pw1.size() == password_len && pw2.find('\0') == std::string::npos;
  }
};

static constexpr auto thread_count{12uz};
static std::vector<Result> thread_output(thread_count);
static std::vector<std::thread> threads(thread_count);

struct find_passwords {
  void operator()(const auto i_out, md5::Message msg, const auto input_size,
                  const auto begin, const auto count) const {
    Result res;
    for (auto i{begin}; i < begin + count && !res.is_complete(); ++i) {
      const auto msg_len{append_digits(msg, input_size, i)};
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
    thread_output[i_out] = res;
  }
};

Result parallel_find_passwords(md5::Message msg, const std::size_t input_size,
                               const std::size_t chunk_size) {
  Result res;
  for (auto i{0uz}; i < 100'000'000 && !res.is_complete();
       i += thread_count * chunk_size) {
    for (auto t{0uz}; t < thread_count; ++t) {
      threads[t] = std::thread(find_passwords{}, t, msg, input_size,
                               i + t * chunk_size, chunk_size);
    }
    for (auto t{0uz}; t < thread_count; ++t) {
      threads[t].join();
      Result th_res{thread_output[t]};
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

  const auto parallel_chunk_size{1uz << 18};
  const auto [part1, part2] =
      parallel_find_passwords(msg, input_size, parallel_chunk_size);
  std::print("{} {}\n", part1, part2);

  return 0;
}
