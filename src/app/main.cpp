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

inline double calcScore(int area1, int area2) {
  double mi = min<int>(area1, area2);
  double ma = max<int>(area1, area2);
  return 1.0 - (1.0 - mi / ma) * (1.0 - mi / ma);
}

vector<int> raiseWeakRect(const vector<Query>& queries, const vector<Rect>& rects) {
  constexpr double Threshold = 0.4;
  const int N = queries.size();
  assert(queries.size() == rects.size());
  vector<int> res;
  repeat(i, N) {
    auto& query = queries[i];
    auto& rect = rects[i];
    if (!rect.in(query.y, query.x)) {
      res.push_back(i);
      continue;
    }
    if (calcScore(rect.w * rect.h, query.r) < Threshold) {
      res.push_back(i);
    };
  }
  return res;
}

double calcScore(const vector<Query>& queries, const vector<Rect>& rects) {
  const int N = queries.size();
  assert(queries.size() == rects.size());
  double total = 0.0;
  repeat(i, N) {
    auto& query = queries[i];
    auto& rect = rects[i];
    if (rect.in(query.y, query.x)) {
      total += calcScore(rect.w * rect.h, query.r);
    }
  }
  return total * 1e4;
}

}  // namespace Algo

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
  constexpr int MaxTime = 5000;
  const int N = queries_.size();
  int tick_score_dump = 2;

  double best_score = Algo::calcScore(queries_, rects_);
  vector<Rect> best_snap = rects_;
  int best_life = 0;

  int lop = 0;
  int time;
  while ((time = timer.toc()) < MaxTime - 10) {
    // TODO: 沙汰の実装。
    // 基準値を設けておき、それに届かないものを消す。
    // 1つ当たりどんなにひどくでも0点、良くても1点
    int i = rand(0, N - 1);
    const auto& query = queries_[i];
    bool expandable = query.r > rects_[i].area();
    // if (!expandable)
    //   continue;
    auto rect_old = rects_[i];
    auto r = rects_[i];
    int amp = 1;
    repeat(_, 99) {
      int k;
      amp = rand(1, 5 + 49 * (MaxTime - time) / MaxTime);
      if (!expandable)
        k = rand(4, 7);
      else
        k = rand(0, 7);
      // TODO: shrink の実装
      if (k == 0) {
        if (r.x <= 0 || !expandable)
          continue;
        r.x -= 1;  // TODO: 移動量を大きくする
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
        if (r.x <= (amp - 1) || r.x + r.w - amp <= query.x)
          continue;
        r.x -= amp;
        break;
      } else if (k == 5) {
        if (r.x + r.w >= WMAX - amp || query.x <= r.x - (amp - 1))
          continue;
        r.x += amp;
        break;
      } else if (k == 6) {
        if (r.y <= (amp - 1) || r.y + r.h - amp <= query.y)
          continue;
        r.y -= amp;
        break;
      } else if (k == 7) {
        if (r.y + r.h >= WMAX - amp || query.y <= r.y - (amp - 1))
          continue;
        r.y += amp;
        break;
      }
    }
    rects_[i] = {-99, -99, 1, 1};
    if (Algo::checkHit(rects_, r)) {
      rects_[i] = rect_old;
    } else {
      rects_[i] = r;
    }
    ++lop;

    if (lop % 10 == 0) {
      double score = Algo::calcScore(queries_, rects_);
      if (best_score < score) {
        best_life = 0;
        best_score = score;
        best_snap = rects_;
      } else {
        ++best_life;
        if (best_life > 200) {
          best_life = 0;
          rects_ = best_snap;
        }
      }
      if (tick_score_dump < timer.toc() / 100) {
        tick_score_dump++;
        clog << best_score << '\n';
      }
    }
    if (lop % 10000 == 0) {
      auto idxs = Algo::raiseWeakRect(queries_, rects_);
      if (!idxs.empty()) {
        // int r = rand(0, int(idxs.size() - 1));
        // shuffle(all(idxs), randdev);
        // rrepeat(r, min<int>(idxs.size(), 5)) {
        //   int i = idxs[r];
        for (int i : idxs) {
          rects_[i].w = 1;
          rects_[i].h = 1;
        }
      }
    }
  }
  {
    double score = Algo::calcScore(queries_, rects_);
    if (best_score > score)
      rects_ = best_snap;
  }
  clog << "lop=" << lop << "\n";
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
  clog << (ll)round(Algo::calcScore(queries, ans)) << endl;
#endif
  return 0;
}
