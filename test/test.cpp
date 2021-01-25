#include <algorithm>
#include <sstream>
#include <string>
#include <xbit_vector/xbit_vector.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace biomodern;

#define EMPTY(...)
#define EMPTYS                                                                 \
  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, \
      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,    \
      EMPTY, EMPTY, EMPTY

#define USE_FIRST_(x, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, \
                   _14, _15, _16, _17, _18, _19, _20, ...)                    \
  _1(x);                                                                      \
  _2(x);                                                                      \
  _3(x);                                                                      \
  _4(x);                                                                      \
  _5(x);                                                                      \
  _6(x);                                                                      \
  _7(x);                                                                      \
  _8(x);                                                                      \
  _9(x);                                                                      \
  _10(x);                                                                     \
  _11(x);                                                                     \
  _12(x);                                                                     \
  _13(x);                                                                     \
  _14(x);                                                                     \
  _15(x);                                                                     \
  _16(x);                                                                     \
  _17(x);                                                                     \
  _18(x);                                                                     \
  _19(x);                                                                     \
  _20(x);

#define MY_MAGIC(f, ...) f(__VA_ARGS__)

#define USE_FIRST(...) MY_MAGIC(USE_FIRST_, __VA_ARGS__, EMPTYS)

#define CHECK_CONTAINER_empty(x)       \
  ;                                    \
  SECTION("check empty.") {            \
    REQUIRE(x.begin() == x.end());     \
    REQUIRE(x.rbegin() == x.rend());   \
    REQUIRE(x.cbegin() == x.cend());   \
    REQUIRE(x.crbegin() == x.crend()); \
    REQUIRE(x.size() == 0);            \
    REQUIRE(x.empty() == true);        \
  }

#define CHECK_COPY_CONSTRUCTION(x1, x2) \
  ;                                     \
  REQUIRE(x1 == x2);

#define CHECK_ALL_ZERO(x)      \
  REQUIRE(x.empty() == false); \
  std::for_each(x.begin(), x.end(), [](auto v) { REQUIRE(v == 0); });
#define CHECK_ALL_ONE(x)       \
  REQUIRE(x.empty() == false); \
  std::for_each(x.begin(), x.end(), [](auto v) { REQUIRE(v == 1); });
#define CHECK_ACCESS_111(x)  \
  SECTION("access") {        \
    REQUIRE(x[0] == 1);      \
    REQUIRE(x[1] == 1);      \
    REQUIRE(x[2] == 1);      \
    REQUIRE(x.at(0) == 1);   \
    REQUIRE(x.at(1) == 1);   \
    REQUIRE(x.at(2) == 1);   \
    REQUIRE(x.front() == 1); \
    REQUIRE(x.back() == 1);  \
  }

#define CHECK_COPY_CONSTRUCTOR(s, x, ...) \
  ;                                       \
  SECTION("copy constructor:" #s) {       \
    auto y(x);                            \
    const auto cy = x;                    \
    CHECK_COPY_CONSTRUCTION(x, y);        \
    CHECK_COPY_CONSTRUCTION(x, cy);       \
    USE_FIRST(y, __VA_ARGS__);            \
    USE_FIRST(cy, __VA_ARGS__);           \
  }

using block_type = std::uint32_t;
using size_type = DibitVector<block_type>::size_type;
using value_type = DibitVector<block_type>::value_type;
using Sequence = DibitVector<block_type>;
using Allocator = std::allocator<block_type>;

TEST_CASE("dibit_vector") {
  size_type size = 3;
  value_type val = 1;
  std::string s(size, val);
  SECTION("default constructor") {
    DibitVector<> v;
    const DibitVector<> cv;
    Sequence s;
    const Sequence cs;
    CHECK_CONTAINER_empty(v);
    CHECK_CONTAINER_empty(cv);
    CHECK_CONTAINER_empty(s);
    CHECK_CONTAINER_empty(cs);

    CHECK_COPY_CONSTRUCTOR("copy which without template parameter", v,
                           CHECK_CONTAINER_empty);

    CHECK_COPY_CONSTRUCTOR("copy which without template parameter, const", cv,
                           CHECK_CONTAINER_empty);
    CHECK_COPY_CONSTRUCTOR("copy which with block type", s,
                           CHECK_CONTAINER_empty);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, const", cs,
                           CHECK_CONTAINER_empty);
  }

  SECTION("allocator constructor") {
    Allocator a;
    const Allocator ca;
    DibitVector<block_type, Allocator> v(a), cv(ca);
    CHECK_CONTAINER_empty(v);
    CHECK_CONTAINER_empty(cv);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, allocator", v,
                           CHECK_CONTAINER_empty);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, const allocator", cv,
                           CHECK_CONTAINER_empty);
  }

  SECTION("size constructor") {
    DibitVector<block_type> v(size);
    const DibitVector<> cv(size);
    REQUIRE(v.size() == size);
    REQUIRE(cv.size() == size);
    CHECK_ALL_ZERO(v);
    CHECK_ALL_ZERO(cv);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, size", v,
                           CHECK_ALL_ZERO);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, size, const", cv,
                           CHECK_ALL_ZERO);
  }

  SECTION("size_value constructor") {
    DibitVector<block_type> v(size, val);
    const DibitVector<> cv(size, val);
    REQUIRE(v.size() == size);
    REQUIRE(cv.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_ALL_ONE(cv);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, size, val", v,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, size, val, const", cv,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
  }
  SECTION("size_value_alloc constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v(size, val, a);
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with block type, size, val, alloc", v,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
  }
  SECTION("size_value_alloc_2 constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v(3, 1, a);
    REQUIRE(v.size() == 3);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with wow size, val, alloc", v,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
  }
  SECTION("range constructor") {
    std::istringstream ss(s);
    DibitVector<> v(std::istream_iterator<char>(ss),
                    std::istream_iterator<char>{});
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with range", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("range_alloc constructor") {
    Allocator a;
    std::istringstream ss(s);
    DibitVector<block_type, Allocator> v(std::istream_iterator<char>(ss),
                                         std::istream_iterator<char>{}, a);
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with range", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("list constructor") {
    DibitVector<> v{1, 1, 1};
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with list", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("list_alloc constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v({1, 1, 1}, a);
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with list_alloc", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("move constructor") {
    DibitVector<> v_{1, 1, 1};
    DibitVector<> v(std::move(v_));
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with move", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("move_alloc constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v_({1, 1, 1}, a);
    DibitVector<block_type> v(std::move(v_));
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with move_alloc", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }
  SECTION("moveAssign_alloc constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v_({1, 1, 1}, a);
    DibitVector<block_type> v = std::move(v_);
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with moveAssign_alloc", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }
  SECTION("iter_alloc constructor") {
    Allocator a;
    DibitVector<block_type, Allocator> v_({1, 1, 1}, a);
    DibitVector<block_type> v(v_.begin(), v_.end());
    REQUIRE(v.size() == size);
    CHECK_ALL_ONE(v);
    CHECK_COPY_CONSTRUCTOR("copy which with iter_alloc", v, CHECK_ALL_ONE,
                           CHECK_ACCESS_111);
  }

  SECTION("assign") {
    std::istringstream ss(s);
    DibitVector<> v1, v2, v3;

    v1.assign(3, 1);
    v2.assign(s.begin(), s.end());
    v3.assign(std::istream_iterator<char>(ss), std::istream_iterator<char>{});
    REQUIRE(v1.size() == size);
    REQUIRE(v2.size() == size);
    REQUIRE(v3.size() == size);
    CHECK_ALL_ONE(v1);
    CHECK_ALL_ONE(v2);
    CHECK_ALL_ONE(v3);
    CHECK_COPY_CONSTRUCTOR("copy which with assign(input iter)", v1,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
    CHECK_COPY_CONSTRUCTOR("copy which with assign(fwd iter)", v2,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
    CHECK_COPY_CONSTRUCTOR("copy which with assign(size val)", v3,
                           CHECK_ALL_ONE, CHECK_ACCESS_111);
  }
  SECTION("reserve") {
    size_t rsv = 1024;
    DibitVector<> v(3, 1);
    v.reserve(rsv);
    REQUIRE(v.capacity() == 1024);
    REQUIRE(v.size() == 3);

    v.shrink_to_fit();

    CHECK_ACCESS_111(v);

    v.clear();
    CHECK_CONTAINER_empty(v);
    v.shrink_to_fit();
    REQUIRE(v.capacity() == 0);
  }
  SECTION("modify") {
    DibitVector<> v;

    SECTION("push_back") {
      CHECK_CONTAINER_empty(v);
      v.push_back(1);
      v.push_back(1);
      v.push_back(1);
      CHECK_ALL_ONE(v);
      CHECK_ACCESS_111(v);
    }

    SECTION("emplace_back") {
      CHECK_CONTAINER_empty(v);
      v.emplace_back(1);
      v.emplace_back(1);
      v.emplace_back(1);
      CHECK_ALL_ONE(v);
      CHECK_ACCESS_111(v);
    }

    SECTION("pop_back") {
      CHECK_CONTAINER_empty(v);
      v.push_back(1);
      v.push_back(1);
      v.push_back(1);
      v.pop_back();
      v.pop_back();
      v.pop_back();
      CHECK_CONTAINER_empty(v);
    }

    SECTION("emplace_insert_erase") {
      CHECK_CONTAINER_empty(v);
      v.assign(3, 3);                               // 3, 3, 3
      v.insert(v.begin(), s.begin() + 1, s.end());  // 1, 1, 3, 3, 3
      v.emplace(v.begin(), 1);                      // 1, 1, 1, 3, 3, 3
      v.emplace(v.end(), 1);                        // 1, 1, 1, 3, 3, 3, 1
      CHECK_ACCESS_111(v);                          // 1, 1, 1, x, ..., x, 1
      v.erase(v.begin(), v.end());
      CHECK_CONTAINER_empty(v);
    }

    SECTION("resize") {
      v.resize(3, 1);
      CHECK_ACCESS_111(v);
      v.resize(0);
      CHECK_CONTAINER_empty(v);
    }

    SECTION("swap") {
      DibitVector<> v2{1, 1, 1};
      CHECK_ACCESS_111(v2);
      CHECK_CONTAINER_empty(v);
      v.swap(v2);
      CHECK_ACCESS_111(v);
      CHECK_CONTAINER_empty(v2);
    }

    SECTION("flip") {
      v.assign({0, 1, 2, 3});
      v.flip();
      REQUIRE(v[0] == 3);
      REQUIRE(v[1] == 2);
      REQUIRE(v[2] == 1);
      REQUIRE(v[3] == 0);
    }
  }
  SECTION("compare") {
    DibitVector<> v1{1, 1, 1};
    DibitVector<> v2{1, 1, 1};
    DibitVector<> v3{1, 2, 3};
    DibitVector<> v4{3, 2, 1};
    DibitVector<> v5{1, 1};
    REQUIRE(v1 == v2);
    REQUIRE(v1 <= v3);
    REQUIRE(v5 <= v3);
    REQUIRE(v4 > v3);
    REQUIRE(v5 < v1);
  }
  SECTION("ranges::sort") {
    DibitVector<> v1{0, 2, 3, 1, 1, 1, 0, 1, 2, 3, 0};
    DibitVector<> v2{0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3};
    std::ranges::sort(v1);
    REQUIRE(v1 == v2);
  }
  SECTION("i++") {
    DibitVector<> v{1, 0, 3};
    REQUIRE(--v[2] == 2);
    REQUIRE(v[2]-- == 2);
    REQUIRE(++v[2] == 2);
    REQUIRE(v[2]++ == 2);
    REQUIRE(v[2] == 3);
    REQUIRE(-v[2] == -3);
  }
}