import std;

struct Object;
struct Array;
using Key = std::string;
using Number = long long;
using Value = std::variant<Key, Number, Object, Array>;

struct Object {
  std::unordered_map<Key, Value> values;
};
struct Array {
  std::vector<Value> values;
};

// clang-format off
std::istream& operator>>(std::istream&, Object&);
std::istream& operator>>(std::istream&, Array&);
std::istream& operator>>(std::istream&, Key&);
std::istream& operator>>(std::istream&, Value&);
// clang-format on

std::istream& operator>>(std::istream& is, Object& o) {
  enum class State {
    init,
    end,
    object,
    items,
    invalid,
  };
  auto state{State::init};
  for (char ch; is && state != State::end;) {
    ch = is.peek();
    switch (std::exchange(state, State::invalid)) {
      case State::init: {
        switch (ch) {
          case '{': {
            is.get();
            state = State::object;
          } break;
        }
      } break;
      case State::object: {
        switch (ch) {
          case '}': {
            is.get();
            state = State::end;
          } break;
          case '"': {
            state = State::items;
          } break;
        }
      } break;
      case State::items: {
        switch (ch) {
          case '}': {
            state = State::object;
          } break;
          case ',': {
            is.get();
            state = State::items;
          } break;
          case '"': {
            Key k;
            Value v;
            if (is >> k && is >> ch && ch == ':' && is >> v) {
              o.values[k] = v;
              state = State::items;
            }
          } break;
        }
      } break;
      case State::end:
      case State::invalid:
      default:
        break;
    }
    if (state == State::invalid) {
      break;
    }
  }
  if (state == State::end) {
    return is;
  }
  throw std::runtime_error("json parse failure");
}

std::istream& operator>>(std::istream& is, Array& array) {
  enum class State {
    init,
    end,
    items,
    invalid,
  };
  auto state{State::init};
  for (char ch; is && state != State::end;) {
    ch = is.peek();
    switch (std::exchange(state, State::invalid)) {
      case State::init: {
        switch (ch) {
          case '[': {
            is.get();
            state = State::items;
          } break;
        }
      } break;
      case State::items: {
        switch (ch) {
          case ']': {
            is.get();
            state = State::end;
          } break;
          case ',': {
            is.get();
            state = State::items;
          } break;
          default: {
            Value v;
            if (is >> v) {
              array.values.push_back(v);
              state = State::items;
            }
          } break;
        }
      } break;
      case State::end:
      case State::invalid:
      default:
        break;
    }
    if (state == State::invalid) {
      break;
    }
  }
  if (state == State::end) {
    return is;
  }
  throw std::runtime_error("failed parsing Array");
}

std::istream& operator>>(std::istream& is, Key& key) {
  char ch;
  if (is >> ch && ch == '"') {
    std::string k;
    while (is >> ch && ch != '"') {
      k.push_back(ch);
    }
    key = k;
    return is;
  }
  throw std::runtime_error("failed parsing Key");
}

std::istream& operator>>(std::istream& is, Value& value) {
  enum class State {
    init,
    end,
    invalid,
  };
  auto state{State::init};
  for (char ch; is && state != State::end;) {
    ch = is.peek();
    switch (std::exchange(state, State::invalid)) {
      case State::init: {
        switch (ch) {
          case '{': {
            Object o;
            if (is >> o) {
              value = o;
              state = State::end;
            }
          } break;
          case '[': {
            Array a;
            if (is >> a) {
              value = a;
              state = State::end;
            }
          } break;
          case '"': {
            Key k;
            if (is >> k) {
              value = k;
              state = State::end;
            }
          } break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case '-': {
            Number n;
            if (is >> n) {
              value = n;
              state = State::end;
            }
          } break;
        }
      } break;
      case State::end:
      case State::invalid:
      default:
        break;
    }
    if (state == State::invalid) {
      break;
    }
  }
  if (state == State::end) {
    return is;
  }
  throw std::runtime_error("failed parsing Value");
}

Number sum(const Key&, std::optional<Key>) {
  return {};
}

Number sum(const Number& n, std::optional<Key>) {
  return n;
}

Number sum(const Value&, std::optional<Key>);

Number sum(const Array& a, std::optional<Key> sentinel) {
  Number result{};
  for (const auto& value : a.values) {
    result += sum(value, sentinel);
  }
  return result;
}

Number sum(const Object& o, std::optional<Key> sentinel = {}) {
  Number result{};
  for (const auto& [key, value] : o.values) {
    if (sentinel && std::holds_alternative<Key>(value)
        && std::get<Key>(value) == *sentinel) {
      return {};
    }
    result += sum(value, sentinel);
  }
  return result;
}

Number sum(const Value& v, std::optional<Key> sentinel) {
  Number result{};
  std::visit([&result, &sentinel](auto&& arg) { result += sum(arg, sentinel); },
             v);
  return result;
}

int main() {
  std::ios_base::sync_with_stdio(false);

  Object o;
  std::cin >> o;

  auto part1{sum(o)};
  auto part2{sum(o, std::optional("red"))};

  std::print("{} {}\n", part1, part2);

  return 0;
}
