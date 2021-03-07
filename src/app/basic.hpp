#ifndef BASIC_HPP__
#define BASIC_HPP__

#ifndef CONCATED_SRC
#include "header.hpp"
#endif

struct Query {
  int y, x, r;

  template <typename I>
  static Query createScan(I& i) {
    int x, y, r;
    i >> x >> y >> r;
    return Query{y, x, r};
  }
  template <typename I>
  void scan(I& i) {
    i >> x >> y >> r;
  }
};

struct Rect {
  int y, x, h, w;
  template <typename O>
  void print(O& o) const {
    o << x << ' ' << y << ' ' << x + w << ' ' << y + h;
  }
  bool crashTo(const Rect& r) const {
    // note: 全て2で割ると理解しやすくなる
    int y1 = y * 2 + h;
    int y2 = r.y * 2 + r.h;
    int x1 = x * 2 + w;
    int x2 = r.x * 2 + r.w;
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    return dx < (w + r.w) && dy < (h + r.h);
  }
  inline int area() const { return w * h; }
  inline bool in(int py, int px) const { return y <= py && py < py + h && x <= px && px < x + w; }
};
template <typename O>
O& operator<<(O& o, const Rect& rect) {
  rect.print(o);
  return o;
}

#endif  // BASIC_HPP__
