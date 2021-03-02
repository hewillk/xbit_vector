#pragma once

#include <cassert>
#include <climits>
#include <concepts>
#include <limits>
#include <stdexcept>

namespace biomodern::detail {

template <std::size_t N, std::unsigned_integral Block>
class XbitReference {
 public:
  constexpr static Block mask =
      (std::numeric_limits<Block>::max() >> (sizeof(Block) * CHAR_BIT - N));

 private:
  Block* seg_;
  const std::size_t shift_;

 public:
  constexpr XbitReference(Block* seg, std::size_t offset) noexcept
      : seg_(seg), shift_(offset * N) {}

  constexpr operator std::uint8_t() const noexcept { return *seg_ >> shift_ & mask; }

  constexpr XbitReference& operator++() noexcept { return operator=(*this + 1); }

  constexpr std::uint8_t operator++(int) noexcept {
    std::uint8_t tmp = *this;
    ++*this;
    return tmp;
  }

  constexpr XbitReference& operator--() noexcept { return operator=(*this - 1); }

  constexpr std::uint8_t operator--(int) noexcept {
    std::uint8_t tmp = *this;
    --*this;
    return tmp;
  }

  constexpr XbitReference& operator=(std::uint8_t x) noexcept {
    *seg_ &= ~(mask << shift_);
    *seg_ |= (x & mask) << shift_;
    return *this;
  }

  constexpr XbitReference& operator=(const XbitReference& x) noexcept {
    return operator=(static_cast<std::uint8_t>(x));
  }

  constexpr void operator=(std::uint8_t) const noexcept {}
};

template <std::size_t N, std::unsigned_integral Block>
inline void swap(XbitReference<N, Block> x, XbitReference<N, Block> y) noexcept {
  std::uint8_t t = x;
  x = y;
  y = t;
}

template <std::size_t N, std::unsigned_integral Block>
inline void swap(XbitReference<N, Block> x, std::uint8_t& y) noexcept {
  std::uint8_t t = x;
  x = y;
  y = t;
}

template <std::size_t N, std::unsigned_integral Block>
inline void swap(std::uint8_t& x, XbitReference<N, Block> y) noexcept {
  std::uint8_t t = x;
  x = y;
  y = t;
}

template <std::size_t N, std::unsigned_integral Block>
class XbitIteratorBase {
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef std::uint8_t value_type;
  typedef std::ptrdiff_t difference_type;
  typedef void pointer;

  constexpr static std::size_t xbits_per_block = sizeof(Block) * CHAR_BIT / N;

 protected:
  Block* seg_;
  std::size_t offset_;

 public:
  constexpr XbitIteratorBase(Block* seg, std::size_t offset) noexcept
      : seg_(seg), offset_(offset) {}

  constexpr friend difference_type operator-(const XbitIteratorBase& x, const XbitIteratorBase& y) {
    return (x.seg_ - y.seg_) * xbits_per_block + x.offset_ - y.offset_;
  }

  constexpr bool operator==(const XbitIteratorBase& other) const noexcept = default;

  constexpr auto operator<=>(const XbitIteratorBase& other) const noexcept {
    if (auto cmp = seg_ <=> other.seg_; cmp != 0) return cmp;
    return offset_ <=> other.offset_;
  }

 protected:
  constexpr void bump_up() {
    if (offset_ != xbits_per_block - 1) ++offset_;
    else {
      offset_ = 0;
      ++seg_;
    }
  }

  constexpr void bump_down() {
    if (offset_ != 0) --offset_;
    else {
      offset_ = xbits_per_block - 1;
      --seg_;
    }
  }

  constexpr void incr(difference_type n) {
    if (n >= 0) seg_ += (n + offset_) / xbits_per_block;
    else
      seg_ += static_cast<difference_type>(n - xbits_per_block + offset_ + 1) /
              static_cast<difference_type>(xbits_per_block);
    n &= (xbits_per_block - 1);
    offset_ = (n + offset_) & (xbits_per_block - 1);
  }
};

template <std::size_t N, std::unsigned_integral Block>
struct XbitIterator : public XbitIteratorBase<N, Block> {
  typedef XbitIteratorBase<N, Block> Base;
  typedef Base::iterator_category iterator_category;
  typedef Base::value_type value_type;
  typedef Base::difference_type difference_type;
  typedef Base::pointer pointer;
  typedef XbitReference<N, Block> reference;
  typedef XbitIterator iterator;

  template <std::size_t, std::unsigned_integral>
  friend class XbitConstIterator;

  constexpr XbitIterator() noexcept : XbitIteratorBase<N, Block>(nullptr, 0) {}

  constexpr XbitIterator(Block* seg, std::size_t offset) noexcept
      : XbitIteratorBase<N, Block>(seg, offset) {}

  constexpr reference operator*() const noexcept { return reference(this->seg_, this->offset_); }

  constexpr reference operator[](difference_type n) const { return *(*this + n); }

  constexpr iterator& operator++() {
    this->bump_up();
    return *this;
  }

  constexpr iterator operator++(int) {
    iterator tmp = *this;
    this->bump_up();
    return tmp;
  }

  constexpr iterator& operator--() {
    this->bump_down();
    return *this;
  }

  constexpr iterator operator--(int) {
    iterator tmp = *this;
    this->bump_down();
    return tmp;
  }

  constexpr iterator& operator+=(difference_type n) {
    this->incr(n);
    return *this;
  }

  constexpr iterator& operator-=(difference_type n) { return *this += -n; }

  constexpr iterator operator+(difference_type n) const {
    iterator tmp(*this);
    tmp += n;
    return tmp;
  }

  constexpr iterator operator-(difference_type n) const {
    iterator tmp(*this);
    tmp -= n;
    return tmp;
  }

  constexpr friend iterator operator+(difference_type n, const iterator& it) { return it + n; }
};

template <std::size_t N, std::unsigned_integral Block>
struct XbitConstIterator : public XbitIteratorBase<N, Block> {
  typedef XbitIteratorBase<N, Block> Base;
  typedef Base::iterator_category iterator_category;
  typedef Base::value_type value_type;
  typedef Base::difference_type difference_type;
  typedef Base::pointer pointer;
  typedef value_type reference;
  typedef value_type const_reference;
  typedef XbitConstIterator const_iterator;

  constexpr XbitConstIterator() noexcept : XbitIteratorBase<N, Block>(nullptr, 0) {}

  constexpr XbitConstIterator(Block* seg, std::size_t offset) noexcept
      : XbitIteratorBase<N, Block>(seg, offset) {}

  constexpr XbitConstIterator(const XbitIterator<N, Block>& x) noexcept
      : XbitIteratorBase<N, Block>(x.seg_, x.offset_) {}

  constexpr const_reference operator*() const noexcept {
    return XbitReference<N, Block>(this->seg_, this->offset_);
  }

  constexpr const_reference operator[](difference_type n) const { return *(*this + n); }

  constexpr const_iterator& operator++() {
    this->bump_up();
    return *this;
  }

  constexpr const_iterator operator++(int) {
    const_iterator tmp = *this;
    this->bump_up();
    return tmp;
  }

  constexpr const_iterator& operator--() {
    this->bump_down();
    return *this;
  }

  constexpr const_iterator operator--(int) {
    const_iterator tmp = *this;
    this->bump_down();
    return tmp;
  }

  constexpr const_iterator& operator+=(difference_type n) {
    this->incr(n);
    return *this;
  }

  constexpr const_iterator& operator-=(difference_type n) { return *this += -n; }

  constexpr const_iterator operator+(difference_type n) const {
    const_iterator tmp(*this);
    tmp += n;
    return tmp;
  }

  constexpr const_iterator operator-(difference_type n) const {
    const_iterator tmp(*this);
    tmp -= n;
    return tmp;
  }

  constexpr friend const_iterator operator+(difference_type n, const const_iterator& it) {
    return it + n;
  }
};

class XbitVectorBase {
 protected:
  constexpr XbitVectorBase() = default;

  constexpr void throw_length_error() const {
    if (!std::is_constant_evaluated()) throw std::length_error("XbitVector");
  }

  constexpr void throw_out_of_range() const {
    if (!std::is_constant_evaluated()) throw std::out_of_range("XbitVector");
  }
};

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
requires(!std::same_as<Block, bool>) class XbitVector : private detail::XbitVectorBase {
 public:
  typedef std::uint8_t value_type;
  typedef Block block_type;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef detail::XbitReference<N, block_type> reference;
  typedef value_type const_reference;
  typedef detail::XbitIterator<N, block_type> iterator;
  typedef detail::XbitConstIterator<N, block_type> const_iterator;
  typedef iterator pointer;
  typedef const value_type* const_pointer;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  typedef Allocator allocator_type;
  typedef std::allocator_traits<allocator_type> allocator_traits;

  constexpr static std::size_t xbits_per_block = iterator::xbits_per_block;

 private:
  block_type* begin_{};
  size_type size_{};
  size_type cap_{};
  /*[[no_unique_address]]*/ allocator_type alloc_{};

 public:
  constexpr XbitVector() noexcept(std::is_nothrow_default_constructible_v<allocator_type>);

  constexpr explicit XbitVector(const allocator_type& a) noexcept;

  constexpr ~XbitVector();

  constexpr explicit XbitVector(size_type n);

  constexpr explicit XbitVector(size_type n, const allocator_type& a);

  constexpr XbitVector(size_type n, const value_type& x);

  constexpr XbitVector(size_type n, const value_type& x, const allocator_type& a);

  constexpr XbitVector(std::input_iterator auto first, std::input_iterator auto last);

  constexpr XbitVector(
      std::input_iterator auto first, std::input_iterator auto last, const allocator_type& a);

  constexpr XbitVector(std::forward_iterator auto first, std::forward_iterator auto last);

  constexpr XbitVector(
      std::forward_iterator auto first, std::forward_iterator auto last, const allocator_type& a);

  constexpr XbitVector(const XbitVector& v);

  constexpr XbitVector(const XbitVector& v, const allocator_type& a);

  constexpr XbitVector& operator=(const XbitVector& v);

  constexpr XbitVector(std::initializer_list<value_type> il);

  constexpr XbitVector(std::initializer_list<value_type> il, const allocator_type& a);

  constexpr XbitVector(XbitVector&& v) noexcept;

  constexpr XbitVector(XbitVector&& v, const allocator_type& a);

  constexpr XbitVector& operator=(XbitVector&& v) noexcept(
      allocator_traits::propagate_on_container_move_assignment::value ||
      allocator_traits::is_always_equal::value);

  constexpr XbitVector& operator=(std::initializer_list<value_type> il) {
    assign(il.begin(), il.end());
    return *this;
  }

  constexpr void assign(std::input_iterator auto first, std::input_iterator auto last);

  constexpr void assign(std::forward_iterator auto first, std::forward_iterator auto last);

  constexpr void assign(size_type n, const value_type& x);

  constexpr void assign(std::initializer_list<value_type> il) { assign(il.begin(), il.end()); }

  constexpr allocator_type get_allocator() const noexcept { return allocator_type(this->alloc_); }

  constexpr size_type max_size() const noexcept;

  constexpr size_type capacity() const noexcept { return internal_cap_to_external(cap_); }

  constexpr size_type size() const noexcept { return size_; }

  constexpr size_type num_blocks() const noexcept {
    return empty() ? 0 : external_cap_to_internal(size());
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }

  constexpr void reserve(size_type n);

  constexpr void shrink_to_fit() noexcept;

  constexpr iterator begin() noexcept { return make_iter(0); }

  constexpr const_iterator begin() const noexcept { return make_iter(0); }

  constexpr iterator end() noexcept { return make_iter(size_); }

  constexpr const_iterator end() const noexcept { return make_iter(size_); }

  constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

  constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

  constexpr const_iterator cbegin() const noexcept { return make_iter(0); }

  constexpr const_iterator cend() const noexcept { return make_iter(size_); }

  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  constexpr reference operator[](size_type n) { return *make_iter(n); }

  constexpr const_reference operator[](size_type n) const { return *make_iter(n); }

  constexpr reference at(size_type n);

  constexpr const_reference at(size_type n) const;

  constexpr reference front() { return *begin(); }

  constexpr const_reference front() const { return *begin(); }

  constexpr reference back() { return *(end() - 1); }

  constexpr const_reference back() const { return *(end() - 1); }

  constexpr block_type* data() noexcept { return begin_; }

  constexpr const block_type* data() const noexcept { return begin_; }

  constexpr void push_back(const value_type& x);

  template <typename... Args>
  constexpr reference emplace_back(Args&&... args) {
    push_back(value_type(std::forward<Args>(args)...));
    return this->back();
  }

  constexpr void pop_back() { --size_; }

  template <typename... Args>
  constexpr iterator emplace(const_iterator position, Args&&... args) {
    return insert(position, value_type(std::forward<Args>(args)...));
  }

  constexpr iterator insert(const_iterator position, const value_type& x);

  constexpr iterator insert(const_iterator position, size_type n, const value_type& x);

  constexpr iterator insert(
      const_iterator position, std::input_iterator auto first, std::input_iterator auto last);

  constexpr iterator insert(
      const_iterator position, std::forward_iterator auto first, std::forward_iterator auto last);

  constexpr iterator insert(const_iterator position, std::initializer_list<value_type> il) {
    return insert(position, il.begin(), il.end());
  }

  constexpr iterator erase(const_iterator position);

  constexpr iterator erase(const_iterator first, const_iterator last);

  constexpr void clear() noexcept { size_ = 0; }

  constexpr void swap(XbitVector&) noexcept;

  constexpr void resize(size_type sz, value_type x = 0);

  constexpr void flip() noexcept;

  constexpr bool operator==(const XbitVector& other) const {
    return size() == other.size() && std::equal(begin(), end(), other.begin());
  }

  constexpr auto operator<=>(const XbitVector& other) const {
    return std::lexicographical_compare_three_way(begin(), end(), other.begin(), other.end());
  }

 private:
  constexpr bool invariants() const;

  constexpr void invalidate_all_iterators();

  constexpr void vallocate(size_type n);

  constexpr void vdeallocate() noexcept;

  constexpr static size_type internal_cap_to_external(size_type n) noexcept {
    return n * xbits_per_block;
  }

  constexpr static size_type external_cap_to_internal(size_type n) noexcept {
    return (n - 1) / xbits_per_block + 1;
  }

  constexpr static size_type align_it(size_type new_size) noexcept {
    return (new_size + (xbits_per_block - 1)) / xbits_per_block * xbits_per_block;
  }

  constexpr size_type recommend(size_type new_size) const;

  constexpr void construct_at_end(size_type n, value_type x);

  constexpr void construct_at_end(
      std::forward_iterator auto first, std::forward_iterator auto last);

  constexpr iterator make_iter(size_type pos) noexcept {
    return iterator(begin_ + pos / xbits_per_block, pos & (xbits_per_block - 1));
  }

  constexpr const_iterator make_iter(size_type pos) const noexcept {
    return const_iterator(begin_ + pos / xbits_per_block, pos & (xbits_per_block - 1));
  }

  constexpr iterator const_iterator_cast(const_iterator p) noexcept {
    return begin() + (p - cbegin());
  }

  constexpr void copy_assign_alloc(const XbitVector& v) {
    if constexpr (allocator_traits::propagate_on_container_copy_assignment::value) {
      if (alloc_ != v.alloc_) vdeallocate();
      alloc_ = v.alloc_;
    }
  }

  constexpr void move_assign(XbitVector& v) noexcept(
      std::is_nothrow_move_assignable_v<allocator_type>);

  constexpr void move_assign_alloc(XbitVector& v) noexcept(
      !allocator_traits::propagate_on_container_move_assignment::value ||
      std::is_nothrow_move_assignable_v<allocator_type>) {
    if constexpr (allocator_traits::propagate_on_container_move_assignment::value)
      alloc_ = std::move(v.alloc_);
  }
};

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::invalidate_all_iterators() {}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::vallocate(size_type n) {
  if (n > max_size()) this->throw_length_error();
  n = external_cap_to_internal(n);
  this->begin_ = allocator_traits::allocate(this->alloc_, n);
  this->size_ = 0;
  this->cap_ = n;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::vdeallocate() noexcept {
  if (this->begin_ != nullptr) {
    allocator_traits::deallocate(this->alloc_, this->begin_, this->cap_);
    invalidate_all_iterators();
    this->begin_ = nullptr;
    this->size_ = this->cap_ = 0;
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::size_type XbitVector<N, Block, Allocator>::max_size()
    const noexcept {
  size_type amax = allocator_traits::max_size(alloc_);
  size_type nmax = std::numeric_limits<size_type>::max() / 2;
  if (nmax / xbits_per_block <= amax) return nmax;
  return internal_cap_to_external(amax);
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::size_type XbitVector<N, Block, Allocator>::recommend(
    size_type new_size) const {
  const size_type ms = max_size();
  if (new_size > ms) this->throw_length_error();
  const size_type cap = capacity();
  if (cap >= ms / 2) return ms;
  return std::max(2 * cap, align_it(new_size));
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::construct_at_end(size_type n, value_type x) {
  size_type old_size = this->size_;
  this->size_ += n;
  std::fill_n(make_iter(old_size), n, x);
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::construct_at_end(
    std::forward_iterator auto first, std::forward_iterator auto last) {
  size_type old_size = this->size_;
  this->size_ += std::distance(first, last);
  std::copy(first, last, make_iter(old_size));
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector() noexcept(
    std::is_nothrow_default_constructible_v<allocator_type>) {}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(const allocator_type& a) noexcept
    : cap_(0), alloc_(a) {}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(size_type n) {
  if (n > 0) {
    vallocate(n);
    construct_at_end(n, 0);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(size_type n, const allocator_type& a)
    : cap_(0), alloc_(a) {
  if (n > 0) {
    vallocate(n);
    construct_at_end(n, 0);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(size_type n, const value_type& x) {
  if (n > 0) {
    vallocate(n);
    construct_at_end(n, x);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    size_type n, const value_type& x, const allocator_type& a)
    : cap_(0), alloc_(a) {
  if (n > 0) {
    vallocate(n);
    construct_at_end(n, x);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    std::input_iterator auto first, std::input_iterator auto last) {
  try {
    for (; first != last; ++first) push_back(*first);
  } catch (...) {
    if (begin_ != nullptr) allocator_traits::deallocate(alloc_, begin_, cap_);
    invalidate_all_iterators();
    throw;
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    std::input_iterator auto first, std::input_iterator auto last, const allocator_type& a)
    : cap_(0), alloc_(a) {
  try {
    for (; first != last; ++first) push_back(*first);
  } catch (...) {
    if (begin_ != nullptr) allocator_traits::deallocate(alloc_, begin_, cap_);
    invalidate_all_iterators();
    throw;
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    std::forward_iterator auto first, std::forward_iterator auto last) {
  const size_type n = std::distance(first, last);
  if (n > 0) {
    vallocate(n);
    construct_at_end(first, last);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    std::forward_iterator auto first, std::forward_iterator auto last, const allocator_type& a)
    : cap_(0), alloc_(a) {
  const size_type n = std::distance(first, last);
  if (n > 0) {
    vallocate(n);
    construct_at_end(first, last);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(std::initializer_list<value_type> il) {
  const size_type n = il.size();
  if (n > 0) {
    vallocate(n);
    construct_at_end(il.begin(), il.end());
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(
    std::initializer_list<value_type> il, const allocator_type& a)
    : cap_(0), alloc_(a) {
  const size_type n = il.size();
  if (n > 0) {
    vallocate(n);
    construct_at_end(il.begin(), il.end());
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::~XbitVector() {
  if (begin_ != nullptr) allocator_traits::deallocate(alloc_, begin_, cap_);
  invalidate_all_iterators();
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(const XbitVector& v)
    : cap_(0), alloc_(allocator_traits::select_on_container_copy_construction(v.alloc_)) {
  if (v.size() > 0) {
    vallocate(v.size());
    construct_at_end(v.begin(), v.end());
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(const XbitVector& v, const allocator_type& a)
    : cap_(0), alloc_(a) {
  if (v.size() > 0) {
    vallocate(v.size());
    construct_at_end(v.begin(), v.end());
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>& XbitVector<N, Block, Allocator>::operator=(
    const XbitVector& v) {
  if (this != &v) {
    copy_assign_alloc(v);
    if (v.size_) {
      if (v.size_ > capacity()) {
        vdeallocate();
        vallocate(v.size_);
      }
      std::copy(v.begin_, v.begin_ + external_cap_to_internal(v.size_), begin_);
    }
    size_ = v.size_;
  }
  return *this;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(XbitVector&& v) noexcept
    : begin_(v.begin_), size_(v.size_), cap_(v.cap_), alloc_(v.alloc_) {
  v.begin_ = nullptr;
  v.size_ = 0;
  v.cap_ = 0;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::XbitVector(XbitVector&& v, const allocator_type& a)
    : cap_(0), alloc_(a) {
  if (a == allocator_type(v.alloc_)) {
    this->begin_ = v.begin_;
    this->size_ = v.size_;
    this->cap_ = v.cap_;
    v.begin_ = nullptr;
    v.cap_ = v.size_ = 0;
  } else if (v.size() > 0) {
    vallocate(v.size());
    construct_at_end(v.begin(), v.end());
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>&
XbitVector<N, Block, Allocator>::operator=(XbitVector&& v) noexcept(
    allocator_traits::propagate_on_container_move_assignment::value ||
    allocator_traits::is_always_equal::value) {
  if constexpr (allocator_traits::propagate_on_container_move_assignment::value) move_assign(v);
  else {
    if (alloc_ != v.alloc_) assign(v.begin(), v.end());
    else
      move_assign(v);
  }
  return *this;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::move_assign(XbitVector& c) noexcept(
    std::is_nothrow_move_assignable_v<allocator_type>) {
  vdeallocate();
  move_assign_alloc(c);
  this->begin_ = c.begin_;
  this->size_ = c.size_;
  this->cap_ = c.cap_;
  c.begin_ = nullptr;
  c.cap_ = c.size_ = 0;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::assign(size_type n, const value_type& x) {
  size_ = 0;
  if (n > 0) {
    size_type c = capacity();
    if (n <= c) size_ = n;
    else {
      XbitVector v(alloc_);
      v.reserve(recommend(n));
      v.size_ = n;
      swap(v);
    }
    std::fill_n(begin(), n, x);
  }
  invalidate_all_iterators();
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::assign(
    std::input_iterator auto first, std::input_iterator auto last) {
  clear();
  for (; first != last; ++first) push_back(*first);
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::assign(
    std::forward_iterator auto first, std::forward_iterator auto last) {
  clear();
  difference_type ns = std::distance(first, last);
  assert(ns >= 0 && "invalid range specified");
  const size_type n = ns;
  if (n) {
    if (n > capacity()) {
      vdeallocate();
      vallocate(n);
    }
    construct_at_end(first, last);
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::reserve(size_type n) {
  if (n > capacity()) {
    XbitVector v(this->alloc_);
    v.vallocate(n);
    v.construct_at_end(this->begin(), this->end());
    swap(v);
    invalidate_all_iterators();
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::shrink_to_fit() noexcept {
  if (external_cap_to_internal(size()) > cap_) {
    try {
      XbitVector(*this, allocator_type(alloc_)).swap(*this);
    } catch (...) {}
  }
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::reference XbitVector<N, Block, Allocator>::at(
    size_type n) {
  if (n >= size()) this->throw_out_of_range();
  return (*this)[n];
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::const_reference XbitVector<N, Block, Allocator>::at(
    size_type n) const {
  if (n >= size()) this->throw_out_of_range();
  return (*this)[n];
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::push_back(const value_type& x) {
  if (this->size_ == this->capacity()) reserve(recommend(this->size_ + 1));
  ++this->size_;
  back() = x;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::iterator XbitVector<N, Block, Allocator>::insert(
    const_iterator position, const value_type& x) {
  iterator r;
  if (size() < capacity()) {
    const_iterator old_end = end();
    ++size_;
    std::copy_backward(position, old_end, end());
    r = const_iterator_cast(position);
  } else {
    XbitVector v(alloc_);
    v.reserve(recommend(size_ + 1));
    v.size_ = size_ + 1;
    r = std::copy(cbegin(), position, v.begin());
    std::copy_backward(position, cend(), v.end());
    swap(v);
  }
  *r = x;
  return r;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::iterator XbitVector<N, Block, Allocator>::insert(
    const_iterator position, size_type n, const value_type& x) {
  iterator r;
  size_type c = capacity();
  if (n <= c && size() <= c - n) {
    const_iterator old_end = end();
    size_ += n;
    std::copy_backward(position, old_end, end());
    r = const_iterator_cast(position);
  } else {
    XbitVector v(alloc_);
    v.reserve(recommend(size_ + n));
    v.size_ = size_ + n;
    r = std::copy(cbegin(), position, v.begin());
    std::copy_backward(position, cend(), v.end());
    swap(v);
  }
  std::fill_n(r, n, x);
  return r;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr typename XbitVector<N, Block, Allocator>::iterator
XbitVector<N, Block, Allocator>::insert(
    const_iterator position, std::input_iterator auto first, std::input_iterator auto last) {
  difference_type off = position - begin();
  iterator p = const_iterator_cast(position);
  iterator old_end = end();
  for (; size() != capacity() && first != last; ++first) {
    ++this->size_;
    back() = *first;
  }
  XbitVector v(alloc_);
  if (first != last) {
    try {
      v.assign(first, last);
      difference_type old_size = old_end - begin();
      difference_type old_p = p - begin();
      reserve(recommend(size() + v.size()));
      p = begin() + old_p;
      old_end = begin() + old_size;
    } catch (...) {
      erase(old_end, end());
      throw;
    }
  }
  p = std::rotate(p, old_end, end());
  insert(p, v.begin(), v.end());
  return begin() + off;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::iterator XbitVector<N, Block, Allocator>::insert(
    const_iterator position, std::forward_iterator auto first, std::forward_iterator auto last) {
  const difference_type n_signed = std::distance(first, last);
  assert(n_signed >= 0 && "invalid range specified");
  const size_type n = n_signed;
  iterator r;
  size_type c = capacity();
  if (n <= c && size() <= c - n) {
    const_iterator old_end = end();
    size_ += n;
    std::copy_backward(position, old_end, end());
    r = const_iterator_cast(position);
  } else {
    XbitVector v(alloc_);
    v.reserve(recommend(size_ + n));
    v.size_ = size_ + n;
    r = std::copy(cbegin(), position, v.begin());
    std::copy_backward(position, cend(), v.end());
    swap(v);
  }
  std::copy(first, last, r);
  return r;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::iterator XbitVector<N, Block, Allocator>::erase(
    const_iterator position) {
  iterator r = const_iterator_cast(position);
  std::copy(position + 1, this->cend(), r);
  --size_;
  return r;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr XbitVector<N, Block, Allocator>::iterator XbitVector<N, Block, Allocator>::erase(
    const_iterator first, const_iterator last) {
  iterator r = const_iterator_cast(first);
  difference_type d = last - first;
  std::copy(last, this->cend(), r);
  size_ -= d;
  return r;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::swap(XbitVector& x) noexcept {
  std::swap(this->begin_, x.begin_);
  std::swap(this->size_, x.size_);
  std::swap(this->cap_, x.cap_);
  if constexpr (allocator_traits::propagate_on_container_swap::value)
    std::swap(this->alloc_, x.alloc_);
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::resize(size_type sz, value_type x) {
  size_type cs = size();
  if (cs < sz) {
    iterator r;
    size_type c = capacity();
    size_type n = sz - cs;
    if (n <= c && cs <= c - n) {
      r = end();
      size_ += n;
    } else {
      XbitVector v(alloc_);
      v.reserve(recommend(size_ + n));
      v.size_ = size_ + n;
      r = std::copy(cbegin(), cend(), v.begin());
      swap(v);
    }
    std::fill_n(r, n, x);
  } else
    size_ = sz;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr void XbitVector<N, Block, Allocator>::flip() noexcept {
  size_type n = 0;
  for (block_type* p = begin_; n < size_; ++p, n += xbits_per_block) *p = ~*p;
}

template <std::size_t N, std::unsigned_integral Block, std::copy_constructible Allocator>
constexpr bool XbitVector<N, Block, Allocator>::invariants() const {
  if (this->begin_ == nullptr) {
    if (this->size_ != 0 || this->cap_ != 0) return false;
  } else {
    if (this->cap_ == 0) return false;
    if (this->size_ > this->capacity()) return false;
  }
  return true;
}

}  // namespace biomodern::detail

namespace biomodern {

template <
    std::unsigned_integral Block = std::uint8_t,
    std::copy_constructible Allocator = std::allocator<Block> >
using DibitVector = detail::XbitVector<2, Block, Allocator>;

template <
    std::unsigned_integral Block = std::uint8_t,
    std::copy_constructible Allocator = std::allocator<Block> >
using QuadbitVector = detail::XbitVector<4, Block, Allocator>;

}  // namespace biomodern
