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
  void print(O& o) {
    o << x << ' ' << y << ' ' << x + h << ' ' << y + w;
  }
  inline bool in(int py, int px) const { return y <= py && py < py + h && x <= px && px < x + w; }
};

//

vector<Rect> solve(const vector<Query>& queries) {
  const int N = queries.size();
  vector<Rect> rects;
  rects.resize(N);
  repeat(i, N) {
    rects[i].x = queries[i].x;
    rects[i].y = queries[i].y;
    rects[i].w = 1;
    rects[i].h = 1;
  }

  return rects;
}

double calcScore(const vector<Query>& queries, const vector<Rect>& rects) {
  const int N = queries.size();
  assert(queries.size() == rects.size());
  double total = 0.0;
  repeat(i, N) {
    auto& query = queries[i];
    auto& rect = rects[i];
    if (rect.in(query.y, query.x)) {
      double s = rect.x * rect.y;
      double mi = min<double>(s, query.r);
      double ma = max<double>(s, query.r);
      total += 1.0 - (1.0 - mi / ma) * (1.0 - mi / ma);
    }
  }
  return total;
}

int main() {
  int N;
  vector<Query> queries;
  vector<Rect> rects;
  scanner >> N;
  queries.clear();
  repeat(i, N) queries.push_back(Query::createScan(scanner));

  auto ans = solve(queries);
  for (auto& r : ans) {
    r.print(printer);
    printer << '\n';
  }
  printer << '\n';
#if 1
  clog << (ll)round(calcScore(queries, ans)) << endl;
#endif
  return 0;
}
