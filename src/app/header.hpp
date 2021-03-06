#ifndef HEADER_HPP__
#define HEADER_HPP__

#pragma GCC optimize ("O3")
#ifdef CONCATED_SRC  // vscodeのIntelliSenseが何故かclangに切り替わって何故か壊れる対策
#pragma GCC target ("avx")
#endif
#include <bits/stdc++.h>

using namespace std;
using ll = long long int;

#define all(v) (v).begin(),(v).end()
#define repeat(cnt,l) for(typename remove_const<typename remove_reference<decltype(l)>::type>::type cnt={};(cnt)<(l);++(cnt))
#define rrepeat(cnt,l) for(auto cnt=(l)-1;0<=(cnt);--(cnt))
#define iterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);++(cnt))
#define diterate(cnt,b,e) for(auto cnt=(b);(cnt)!=(e);--(cnt))
const long long MD = 1000000007ll; const long double PI = 3.1415926535897932384626433832795L;
template<typename T1, typename T2> inline ostream& operator <<(ostream &o, const pair<T1, T2> p) { o << '(' << p.first << ':' << p.second << ')'; return o; }
template<typename T> inline T& chmax(T& to, const T& val) { return to = max(to, val); }
template<typename T> inline T& chmin(T& to, const T& val) { return to = min(to, val); }
void bye(string s, int code = 0) { cout << s << endl; exit(code); }
struct XorShift {
  using result_type = uint64_t;
  result_type x_;
  XorShift(result_type x = 88172645463325252ull) : x_(x){};
  static constexpr inline result_type min() { return 0ull; }
  static constexpr inline result_type max() { return numeric_limits<result_type>::max(); }
  inline result_type operator()() {
    x_ ^= x_ << 7;
    return x_ ^= x_ >> 9;
  }
  inline void discard(unsigned long long z) {
    while (z--)
      operator()();
  }
};
XorShift randdev;
template<typename T, typename Random = decltype(randdev), typename enable_if<is_integral<T>::value>::type* = nullptr>
inline T rand(T l, T h, Random& rand = randdev) { return uniform_int_distribution<T>(l, h)(rand); }
template<typename T, typename Random = decltype(randdev), typename enable_if<is_floating_point<T>::value>::type* = nullptr>
inline T rand(T l, T h, Random& rand = randdev) { return uniform_real_distribution<T>(l, h)(rand); }template<typename T>
static ostream& operator<<(ostream& o, const std::vector<T>& v) {
  o << "[ "; for(const auto& e : v) o<<e<<' ';
  return o << ']';
}

template <typename I>
struct MyRangeFormat{ I b,e; MyRangeFormat(I _b, I _e):b(_b),e(_e){} };
template<typename I>
static ostream& operator<<(ostream& o, const MyRangeFormat<I>& f) {
  o << "[ "; iterate(i,f.b,f.e) o<<*i<<' ';
  return o << ']';
}
template <typename I>
struct MyMatrixFormat{
  const I& p; long long n, m;
  MyMatrixFormat(const I& _p, long long _n, long long _m):p(_p),n(_n),m(_m){}
};
template<typename I>
static ostream& operator<<(ostream& o, const MyMatrixFormat<I>& f) {
  o<<'\n';
  repeat(i,(f.n)) {
    repeat(j,f.m) o<<f.p[i][j]<<' ';
    o<<'\n';
  }
  return o;
}
struct LOG_t { ~LOG_t() { cout << endl; } };
#define LOG (LOG_t(),cout<<'L'<<__LINE__<<": ")
#define FMTA(m,w) (MyRangeFormat<decltype(m+0)>(m,m+w))
#define FMTR(b,e) (MyRangeFormat<decltype(e)>(b,e))
#define FMTV(v) FMTR(v.begin(),v.end())
#define FMTM(m,h,w) (MyMatrixFormat<decltype(m+0)>(m,h,w))

#if defined(_WIN32) || defined(_WIN64)
#define getc_x _getc_nolock
#define putc_x _putc_nolock
#elif defined(__GNUC__)
#define getc_x getc_unlocked
#define putc_x putc_unlocked
#else
#define getc_x getc
#define putc_x putc
#endif
class MaiScanner {
  FILE* fp_;
  constexpr bool isvisiblechar(char c) noexcept { return (0x21<=(c)&&(c)<=0x7E); }
public:
  inline MaiScanner(FILE* fp):fp_(fp){}
  template<typename T> void input_integer(T& var) noexcept {
    var = 0; T sign = 1;
    int cc = getc_x(fp_);
    for (; cc < '0' || '9' < cc; cc = getc_x(fp_))
      if (cc == '-') sign = -1;
    for (; '0' <= cc && cc <= '9'; cc = getc_x(fp_))
      var = (var << 3) + (var << 1) + cc - '0';
    var = var * sign;
  }
  inline int c() noexcept { return getc_x(fp_); }
  template<typename T, typename enable_if<is_integral<T>::value, nullptr_t>::type = nullptr>
  inline MaiScanner& operator>>(T& var) noexcept { input_integer<T>(var); return *this; }
  inline MaiScanner& operator>>(string& var) {
    int cc = getc_x(fp_);
    for (; !isvisiblechar(cc); cc = getc_x(fp_));
    for (; isvisiblechar(cc); cc = getc_x(fp_))
      var.push_back(cc);
    return *this;
  }
  template<typename IT> inline void in(IT begin, IT end) { for (auto it = begin; it != end; ++it) *this >> *it; }
};
class MaiPrinter {
  FILE* fp_;
public:
  inline MaiPrinter(FILE* fp):fp_(fp){}
  template<typename T>
  void output_integer(T var) noexcept {
    if (var == 0) { putc_x('0', fp_); return; }
    if (var < 0)
      putc_x('-', fp_),
      var = -var;
    char stack[32]; int stack_p = 0;
    while (var)
      stack[stack_p++] = '0' + (var % 10),
      var /= 10;
    while (stack_p)
      putc_x(stack[--stack_p], fp_);
  }
  inline MaiPrinter& operator<<(char c) noexcept { putc_x(c, fp_); return *this; }
  template<typename T, typename enable_if<is_integral<T>::value, nullptr_t>::type = nullptr>
  inline MaiPrinter& operator<<(T var) noexcept { output_integer<T>(var); return *this; }
  inline MaiPrinter& operator<<(char* str_p) noexcept { while (*str_p) putc_x(*(str_p++), fp_); return *this; }
  inline MaiPrinter& operator<<(const string& str) {
    const char* p = str.c_str();
    const char* l = p + str.size();
    while (p < l) putc_x(*p++, fp_);
    return *this;
  }
  template<typename IT> void join(IT begin, IT end, char sep = ' ') { for (bool b = 0; begin != end; ++begin, b = 1) b ? *this << sep << *begin : *this << *begin; }
};
MaiScanner scanner(stdin);
MaiPrinter printer(stdout);

#endif  // HEADER_HPP__
