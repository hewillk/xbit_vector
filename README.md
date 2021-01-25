## Biomodern-XbitVector
`Biomodern.DibitVector` and `QuadVector` is a space efficient container for dibit and quadbit values, you can think of it as a specialization of `std::vector` for type `uint2_t` and `uint4_t`.

The implementation detail is refer to [GCC][GCC] and [Clang][Clang]'s implementation of [`vector<bool>`][vector_of_bool] with adopt new C++20 technology such as [`iterator concepts`][iterator_concept], `operator<=>` etc.
  
The synopsis of those two containers is:
```cpp
template <
  std::unsigned_integral Block = std::uint8_t,
  std::copy_constructible Allocator = std::allocator<Block> 
>
class DibitVector;
```
which `Block` refers to the underlying storage type, the default type is `uint8_t` which can store 4 dibits or 2 quadbits in one block. 

The member types and functions definition are just same as [`vector<bool>`][vector_of_bool] with following additions:
```cpp
block_type; // alias to Block.
size_type num_blocks(); // return the number of underlying blocks.
block_type* data();
block_type* data() const; // return the begin pointer to the underlying blocks.
void flip(); // flip all the dibits/quadbits of the vector.
```
Like `vector<bool>`, those two containers can work with all algorithms in [`<algorithm>`][algorithm] even if [`ranges::sort`][ranges_sort] which cannot sort `vector<bool>` currently. Note that the `value_type` of those two containers is `uint8_t` which is not printable character, make sure cast it to `int` before you print:
```cpp
std::cout << static_cast<int>(v.front()) << "\n";
std::cout << +v.back() << "\n";
```
If you want to serialize those two containers, you can use my other project [`Biomodern.Serializer`][Biomodern.Serializer] which proves a simple interface to serialize binary archive.

## Compilers
- GCC 10.2

## Usage
This project is header-only and has no dependency with other libraries, if you want use it, just [copy and paste][godbolt].
```cpp
#include <algorithm>
#include <iostream>
#include <ranges>
#include "xbit_vector/xbit_vector.hpp"

int main() {
  biomodern::DibitVector<> v{3, 2, 1, 2, 3, 0, 0, 1, 2};
  assert(v.size() == 9);
  assert(v.num_blocks() == 3);

  auto base_view = std::views::transform([](auto c){ return "ACGT"[c]; });
  auto comp_view = std::views::transform([](auto c){ return 0b11 - c; });
  std::cout << "Reverse complement of ";
  std::ranges::copy(v | base_view, std::ostream_iterator<char>{std::cout, ""});
  std::cout << " is ";
  for (auto c : v | std::views::reverse | comp_view | base_view)
    std::cout << c;
}
```

## Building and Testing
```
$ mkdir build && cd build
$ make
$ make test
```
[GCC]: https://github.com/gcc-mirror/gcc/blob/master/libstdc%2B%2B-v3/include/bits/stl_bvector.h
[Clang]: https://github.com/llvm-mirror/libcxx/blob/master/include/__bit_reference
[vector_of_bool]: https://en.cppreference.com/w/cpp/container/vector_bool
[iterator_concept]: https://en.cppreference.com/w/cpp/iterator
[algorithm]: https://en.cppreference.com/w/cpp/algorithm
[ranges_sort]: https://godbolt.org/z/xb1195
[Biomodern.Serializer]: https://github.com/hewillk/serializer
[godbolt]: https://godbolt.org/z/YM5P61
