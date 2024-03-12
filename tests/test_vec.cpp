import std;
import aoc;

using std::operator""s;

void assert(bool exp, std::string&& msg) {
  if (not exp) {
    throw std::runtime_error(msg);
  }
}

void assert_equal(auto&& lhs, auto&& rhs, std::string&& msg) {
  assert(lhs == rhs, std::format("{}: expected {} and {} to compare equal", msg, lhs, rhs));
}

template <typename T>
void test_vec1() {
  {
    aoc::Vec1<T> v;
    assert(v.x() == T{}, std::format("vec1: default init x should be {}", T{}));
  }
  {
    aoc::Vec1<T> v(1);
    assert(v.x() == 1, "vec1(1) => x should be 1");
  }
  {
    aoc::Vec1<T> v(1);
    assert_equal(
        std::format("{}", v),
        std::format("Vec1{}", std::tuple{1}),
        "unexpected aoc::Vec format"
    );
  }
  {
    aoc::Vec1<T> lhs, rhs;
    assert((lhs <=> rhs) == 0, "vec1() <=> vec1() should compare equal");
  }
  {
    aoc::Vec1<T> lhs(1), rhs(1);
    assert((lhs <=> rhs) == 0, "vec1(1) <=> vec1(1) should compare equal");
  }
  {
    aoc::Vec1<T> lhs(1), rhs(2);
    assert((lhs <=> rhs) < 0, "vec1(1) <=> vec1(2) should compare less than");
  }
  {
    aoc::Vec1<T> lhs(2), rhs(1);
    assert((lhs <=> rhs) > 0, "vec1(2) <=> vec1(1) should compare greater than");
  }
  {
    aoc::Vec1<T> lhs(6), rhs(-2);
    lhs += rhs;
    assert_equal(lhs, aoc::Vec1<T>(4), "vec1(6) + vec1(-2)");
  }
  {
    aoc::Vec1<T> lhs(2), rhs(6);
    lhs -= rhs;
    assert_equal(lhs, aoc::Vec1<T>(-4), "vec1(2) - vec1(6)");
  }
  {
    aoc::Vec1<T> lhs(6), rhs(-2);
    lhs *= rhs;
    assert_equal(lhs, aoc::Vec1<T>(-12), "vec1(6) * vec1(-2)");
  }
  {
    aoc::Vec1<T> lhs(-6), rhs(-2);
    lhs /= rhs;
    assert_equal(lhs, aoc::Vec1<T>(3), "vec1(-6) / vec1(-2)");
  }
  {
    std::istringstream is{"1"};
    aoc::Vec1<T> v;
    is >> v;
    assert_equal(v, aoc::Vec1<T>(1), "parse string 1");
  }
}

template <typename T>
void test_vec2() {
  {
    aoc::Vec2<T> v;
    assert(v.x() == T{}, std::format("vec2: default init x should be {}", T{}));
    assert(v.y() == T{}, std::format("vec2: default init y should be {}", T{}));
  }
  {
    aoc::Vec2<T> v(1, 2);
    assert(v.x() == 1, "vec2(1, 2) => x should be 1");
    assert(v.y() == 2, "vec2(1, 2) => y should be 2");
  }
  {
    aoc::Vec2<T> v(1, 2);
    assert_equal(
        std::format("{}", v),
        std::format("Vec2{}", std::tuple{1, 2}),
        "unexpected aoc::Vec format"
    );
  }
  {
    aoc::Vec2<T> lhs, rhs;
    assert((lhs <=> rhs) == 0, "vec2() <=> vec2() should compare equal");
  }
  {
    aoc::Vec2<T> lhs(1, 1), rhs(1, 1);
    assert((lhs <=> rhs) == 0, "vec2(1, 1) <=> vec2(1, 1) should compare equal");
  }
  {
    aoc::Vec2<T> lhs(1, 1), rhs(1, 2);
    assert((lhs <=> rhs) < 0, "vec2(1, 1) <=> vec2(1, 2) should compare less than");
  }
  {
    aoc::Vec2<T> lhs(1, 1), rhs(2, 2);
    assert((lhs <=> rhs) < 0, "vec2(1, 1) <=> vec2(2, 2) should compare less than");
  }
  {
    aoc::Vec2<T> lhs(1, 2), rhs(1, 1);
    assert((lhs <=> rhs) > 0, "vec2(1, 2) <=> vec2(1, 1) should compare greater than");
  }
  {
    aoc::Vec2<T> lhs(2, 2), rhs(1, 1);
    assert((lhs <=> rhs) > 0, "vec2(2, 2) <=> vec2(1, 1) should compare greater than");
  }
  {
    aoc::Vec2<T> lhs(6, -12), rhs(3, 2);
    lhs += rhs;
    assert_equal(lhs, aoc::Vec2<T>(9, -10), "vec2(6, -12) + vec2(3, 2)");
  }
  {
    aoc::Vec2<T> lhs(6, 12), rhs(-3, 2);
    lhs -= rhs;
    assert_equal(lhs, aoc::Vec2<T>(9, 10), "vec2(6, 12) - vec2(-3, 2)");
  }
  {
    aoc::Vec2<T> lhs(6, 12), rhs(3, -2);
    lhs *= rhs;
    assert_equal(lhs, aoc::Vec2<T>(18, -24), "vec2(6, 12) * vec2(3, -2)");
  }
  {
    aoc::Vec2<T> lhs(6, 12), rhs(3, 2);
    lhs /= rhs;
    assert_equal(lhs, aoc::Vec2<T>(2, 6), "vec2(6, 12) / vec2(3, 2)");
  }
  {
    std::istringstream is{"1, 2"};
    aoc::Vec2<T> v;
    is >> v;
    assert_equal(v, aoc::Vec2<T>(1, 2), "parse string 1, 2");
  }
  {
    std::istringstream is{"1,2"};
    aoc::Vec2<T> v;
    is >> v;
    assert_equal(v, aoc::Vec2<T>(1, 2), "parse string 1,2");
  }
}

template <typename T>
void test_vec3() {
  {
    aoc::Vec3<T> v;
    assert(v.x() == T{}, std::format("vec3: default init x should be {}", T{}));
    assert(v.y() == T{}, std::format("vec3: default init y should be {}", T{}));
    assert(v.z() == T{}, std::format("vec3: default init z should be {}", T{}));
  }
  {
    aoc::Vec3<T> v(1, 2, 3);
    assert(v.x() == 1, "vec3(1, 2, 3) => x should be 1");
    assert(v.y() == 2, "vec3(1, 2, 3) => y should be 2");
    assert(v.z() == 3, "vec3(1, 2, 3) => z should be 3");
  }
  {
    aoc::Vec3<T> v(1, 2, 3);
    assert_equal(
        std::format("{}", v),
        std::format("Vec3{}", std::tuple{1, 2, 3}),
        "unexpected aoc::Vec format"
    );
  }
  {
    aoc::Vec3<T> lhs, rhs;
    assert((lhs <=> rhs) == 0, "vec3() <=> vec3() should compare equal");
  }
  {
    aoc::Vec3<T> lhs(1, 1, 1), rhs(1, 1, 1);
    assert((lhs <=> rhs) == 0, "vec3(1, 1, 1) <=> vec3(1, 1, 1) should compare equal");
  }
  {
    aoc::Vec3<T> lhs(1, 1, 1), rhs(1, 1, 2);
    assert((lhs <=> rhs) < 0, "vec3(1, 1, 1) <=> vec3(1, 1, 2) should compare less than");
  }
  {
    aoc::Vec3<T> lhs(1, 1, 1), rhs(1, 2, 2);
    assert((lhs <=> rhs) < 0, "vec3(1, 1, 1) <=> vec3(1, 2, 2) should compare less than");
  }
  {
    aoc::Vec3<T> lhs(1, 1, 1), rhs(2, 2, 2);
    assert((lhs <=> rhs) < 0, "vec3(1, 1, 1) <=> vec3(2, 2, 2) should compare less than");
  }
  {
    aoc::Vec3<T> lhs(1, 1, 2), rhs(1, 1, 1);
    assert((lhs <=> rhs) > 0, "vec3(1, 1, 2) <=> vec3(1, 1, 1) should compare greater than");
  }
  {
    aoc::Vec3<T> lhs(1, 2, 2), rhs(1, 1, 1);
    assert((lhs <=> rhs) > 0, "vec3(1, 2, 2) <=> vec3(1, 1, 1) should compare greater than");
  }
  {
    aoc::Vec3<T> lhs(2, 2, 2), rhs(1, 1, 1);
    assert((lhs <=> rhs) > 0, "vec3(2, 2, 2) <=> vec3(1, 1, 1) should compare greater than");
  }
  {
    aoc::Vec3<T> lhs(-2, 0, 2), rhs(1, 2, 3);
    lhs += rhs;
    assert_equal(lhs, aoc::Vec3<T>(-1, 2, 5), "vec3(-2, 0, 2) + vec3(1, 2, 3)");
  }
  {
    aoc::Vec3<T> lhs(-2, 0, 2), rhs(1, 2, 3);
    lhs -= rhs;
    assert_equal(lhs, aoc::Vec3<T>(-3, -2, -1), "vec3(-2, 0, 2) - vec3(1, 2, 3)");
  }
  {
    aoc::Vec3<T> lhs(-2, 0, 2), rhs(1, 2, 3);
    lhs *= rhs;
    assert_equal(lhs, aoc::Vec3<T>(-2, 0, 6), "vec3(-2, 0, 2) * vec3(1, 2, 3)");
  }
  {
    aoc::Vec3<T> lhs(-2, 0, 6), rhs(1, 2, 3);
    lhs /= rhs;
    assert_equal(lhs, aoc::Vec3<T>(-2, 0, 2), "vec3(-2, 0, 2) / vec3(1, 2, 3)");
  }
  {
    std::istringstream is{"1, 2, 3"};
    aoc::Vec3<T> v;
    is >> v;
    assert_equal(v, aoc::Vec3<T>(1, 2, 3), "parse string 1, 2, 3");
  }
  {
    std::istringstream is{"1,2,3"};
    aoc::Vec3<T> v;
    is >> v;
    assert_equal(v, aoc::Vec3<T>(1, 2, 3), "parse string 1,2,3");
  }
}

template <typename T>
void test_vec4() {
  {
    aoc::Vec4<T> v;
    assert(v.x() == T{}, std::format("vec4: default init x should be {}", T{}));
    assert(v.y() == T{}, std::format("vec4: default init y should be {}", T{}));
    assert(v.z() == T{}, std::format("vec4: default init z should be {}", T{}));
    assert(v.w() == T{}, std::format("vec4: default init w should be {}", T{}));
  }
  {
    aoc::Vec4<T> v(1, 2, 3, 4);
    assert(v.x() == 1, "vec4(1, 2, 3, 4) => x should be 1");
    assert(v.y() == 2, "vec4(1, 2, 3, 4) => y should be 2");
    assert(v.z() == 3, "vec4(1, 2, 3, 4) => z should be 3");
    assert(v.w() == 4, "vec4(1, 2, 3, 4) => w should be 4");
  }
  {
    aoc::Vec4<T> v(1, 2, 3, 4);
    assert_equal(
        std::format("{}", v),
        std::format("Vec4{}", std::tuple{1, 2, 3, 4}),
        "unexpected aoc::Vec format"
    );
  }
  {
    aoc::Vec4<T> lhs, rhs;
    assert((lhs <=> rhs) == 0, "vec4() <=> vec4() should compare equal");
  }
  {
    aoc::Vec4<T> lhs(10, 10, 10, 10), rhs(2, 2, 2, 2);
    lhs += rhs;
    assert_equal(lhs, aoc::Vec4<T>(12, 12, 12, 12), "vec4(10, 10, 10, 10) + vec4(2, 2, 2, 2)");
  }
  {
    aoc::Vec4<T> lhs(10, 10, 10, 10), rhs(2, 2, 2, 2);
    lhs -= rhs;
    assert_equal(lhs, aoc::Vec4<T>(8, 8, 8, 8), "vec4(10, 10, 10, 10) - vec4(2, 2, 2, 2)");
  }
  {
    aoc::Vec4<T> lhs(10, 10, 10, 10), rhs(2, 2, 2, 2);
    lhs *= rhs;
    assert_equal(lhs, aoc::Vec4<T>(20, 20, 20, 20), "vec4(10, 10, 10, 10) * vec4(2, 2, 2, 2)");
  }
  {
    aoc::Vec4<T> lhs(10, 10, 10, 10), rhs(2, 2, 2, 2);
    lhs /= rhs;
    assert_equal(lhs, aoc::Vec4<T>(5, 5, 5, 5), "vec4(10, 10, 10, 10) / vec4(2, 2, 2, 2)");
  }
}

int main() {
  test_vec1<int>();
  test_vec2<int>();
  test_vec3<int>();
  test_vec4<int>();

  test_vec1<float>();
  test_vec2<float>();
  test_vec3<float>();
  test_vec4<float>();

  test_vec1<std::ptrdiff_t>();
  test_vec2<std::ptrdiff_t>();
  test_vec3<std::ptrdiff_t>();
  test_vec4<std::ptrdiff_t>();

  test_vec1<double>();
  test_vec2<double>();
  test_vec3<double>();
  test_vec4<double>();

  return 0;
}
