#ifndef CONCATED_SRC
#include "header.hpp"
#include "basic.hpp"
#endif
template <typename C = std::chrono::milliseconds>
class Timer {
  std::chrono::system_clock::time_point tp_;

 public:
  static inline auto now() { return std::chrono::system_clock::now(); }
  inline void tic() { tp_ = now(); }
  inline auto toc() const { return std::chrono::duration_cast<C>(now() - tp_).count(); }
  inline Timer() : tp_(now()) {}
};
inline std::ostream& operator<<(std::ostream& o, const Timer<>& t) {
  return o << (long long)t.toc();
}

// ----------------------------------------------------------------------------

constexpr int WMAX = 10000;  // 多分10001

namespace Algo {

bool checkHit(const vector<Rect>& rects, const Rect& rect) {
  for (auto& r : rects) {
    if (rect.crashTo(r))
      return true;
  }
  return false;
}

}  // namespace Algo

double calcScore(const vector<Query>& queries, const vector<Rect>& rects) {
  const int N = queries.size();
  assert(queries.size() == rects.size());
  double total = 0.0;
  repeat(i, N) {
    auto& query = queries[i];
    auto& rect = rects[i];
    if (rect.in(query.y, query.x)) {
      int s = rect.w * rect.h;
      double mi = min<int>(s, query.r);
      double ma = max<int>(s, query.r);
      total += 1.0 - (1.0 - mi / ma) * (1.0 - mi / ma);
    }
  }
  return total * 1e4;
}

// ----------------------------------------------------------------------------

class Solver {
 public:
  Solver(const vector<Query>& queries) : queries_(queries) {}

  void solve();

  vector<Rect> results() const { return rects_; }

 private:
  void solve00(const Timer<>& timer);
  void solve10(const Timer<>& timer);
  void solve20(const Timer<>& timer);

  // TODO: rect + query structure?
  vector<Query> queries_;
  vector<Rect> rects_;
};

void Solver::solve00(const Timer<>& timer) {
  const int N = queries_.size();
  rects_.resize(N);
  repeat(i, N) {
    rects_[i].x = queries_[i].x;
    rects_[i].y = queries_[i].y;
    rects_[i].w = 1;
    rects_[i].h = 1;
  }
}

void Solver::solve10(const Timer<>& timer) {
  const int N = queries_.size();

  vector<int> targets(N);
  iota(all(targets), 0);

  repeat(_, 500000) {
    int vi = rand(0, int(targets.size()) - 1);
    int i = targets[vi];
    const auto& query = queries_[i];
    bool expandable = query.r > rects_[i].area();
    if (!expandable) {
      targets.erase(targets.begin() + vi);
      continue;
    }
    auto rect_old = rects_[i];
    auto r = rects_[i];
    repeat(_, 9) {
      int k = rand(0, 3);
      if (k == 0) {
        if (r.x <= 0)
          continue;
        r.x -= 1;
        r.w += 1;
        break;
      } else if (k == 1) {
        if (r.x + r.w >= WMAX)
          continue;
        r.w += 1;
        break;
      } else if (k == 2) {
        if (r.y <= 0)
          continue;
        r.y -= 1;
        r.h += 1;
        break;
      } else if (k == 3) {
        if (r.y + r.h >= WMAX)
          continue;
        r.h += 1;
        break;
      }
    }
    rects_[i] = {-8, -8, 1, 1};
    if (Algo::checkHit(rects_, r)) {
      rects_[i] = rect_old;
    } else {
      rects_[i] = r;
    }
  }
}

void Solver::solve20(const Timer<>& timer) {
  const int N = queries_.size();
  int tick_score_dump = 2;
  while (timer.toc() < 1990) {
    int i = rand(0, N - 1);
    const auto& query = queries_[i];
    bool expandable = query.r > rects_[i].area();
    // if (!expandable)
    //   continue;
    auto rect_old = rects_[i];
    auto r = rects_[i];
    repeat(_, 99) {
      int k = rand(0, 3);
      // TODO:
      if (!expandable)
        k += 4;
      if (k == 0) {
        if (r.x <= 0 || !expandable)
          continue;
        r.x -= 1;
        r.w += 1;
        break;
      } else if (k == 1) {
        if (r.x + r.w >= WMAX || !expandable)
          continue;
        r.w += 1;
        break;
      } else if (k == 2) {
        if (r.y <= 0 || !expandable)
          continue;
        r.y -= 1;
        r.h += 1;
        break;
      } else if (k == 3) {
        if (r.y + r.h >= WMAX || !expandable)
          continue;
        r.h += 1;
        break;
      } else if (k == 4) {
        if (r.x <= 0 || r.x + r.w - 1 <= query.x)
          continue;
        r.x -= 1;
        break;
      } else if (k == 5) {
        if (r.x + r.w >= WMAX || query.x <= r.x)
          continue;
        r.x += 1;
        break;
      } else if (k == 6) {
        if (r.y <= 0 || r.y + r.h - 1 <= query.y)
          continue;
        r.y -= 1;
        break;
      } else if (k == 7) {
        if (r.y + r.h >= WMAX || query.y <= r.y)
          continue;
        r.y += 1;
        break;
      }
    }
    rects_[i] = {-8, -8, 1, 1};
    if (Algo::checkHit(rects_, r)) {
      rects_[i] = rect_old;
    } else {
      rects_[i] = r;
    }
    if (tick_score_dump < timer.toc() / 100) {
      tick_score_dump++;
      clog << calcScore(queries_, rects_) << '\n';
    }
  }
}

void Solver::solve() {
  Timer<> timer;
  solve00(timer);
  solve10(timer);
  solve20(timer);
}

// ----------------------------------------------------------------------------

int main() {
  int N;
  vector<Query> queries;
  vector<Rect> rects;

  scanner >> N;
  queries.clear();
  repeat(i, N) queries.push_back(Query::createScan(scanner));

  Solver solver(queries);
  solver.solve();
  auto ans = solver.results();
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
