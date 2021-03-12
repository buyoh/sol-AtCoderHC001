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

namespace Param {

// TODO: パラメータ外部入力による自動パラメータ調整機能
int param = 1;

void applyFromCommandLine(int argc, char** argv) {
  for (int i = 1; i + 1 < argc; i += 2) {
    if (string("--param") == argv[i]) {
      param = stoi(argv[i + 1]);
    }
  }
}

}  // namespace Param

// ----------------------------------------------------------------------------

constexpr int WMAX = 10000;  // 多分10001
constexpr double AMPSCORE = 1e4;

namespace Algo {

int checkHitIndex(const vector<Rect>& rects, const Rect& rect) {
  repeat(i, (int)rects.size()) {
    auto& r = rects[i];
    if (rect.crashTo(r))
      return i;
  }
  return -1;
}

bool cachedCheckHit(const vector<Rect>& rects, const Rect& rect, vector<int>& hitting_history) {
  // TODO: 高速化
  // オーダー改善は何も思い浮かばないので、キャッシュ高速化を考えてみる。
  // 何か隣接する矩形にヒットしたら、そのidをstd::vectorに記録しておき、
  // 次はそれを優先的にチェックする。高々長野県程度の隣接数のはずなので、
  // これでも十分高速化が期待できるはずである→そうでもなかった
  for (auto j : hitting_history) {
    if (rect.crashTo(rects[j])) {
      return true;
    }
  }
  int h = Algo::checkHitIndex(rects, rect);
  if (h >= 0) {
    hitting_history.push_back(h);  // note: 重複しない
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
  return total * AMPSCORE;
}

struct ResultMoveAdjacents {
  bool changed;
  Rect act;
  bool never_hit;
};

ResultMoveAdjacents moveAdjacents(const Rect& r, const Query& query, double timeLerp) {
  // TODO: コピーの方が良いか参照の方が良いか
  const bool expandable = query.r >= r.w * (r.h + 1) || query.r >= (r.w + 1) * r.h;
  // const bool expandable = query.r > r.w * r.h;
  int amp = 1;
  int maxAmp = 5 + static_cast<int>(timeLerp * 49);
  repeat(_, 29) {
    int k;
    amp = rand(1, maxAmp);
    if (!expandable)
      k = rand(4, 11);
    else
      k = rand(0, 11);
    if (k == 0) {
      // 拡大の実装
      if (r.x <= 0 || !expandable)
        continue;
      // r.x -= 1;
      // r.w += 1;
      return {true, Rect{r.y, r.x - 1, r.h, r.w + 1}, false};
      break;
    } else if (k == 1) {
      if (r.x + r.w >= WMAX || !expandable)
        continue;
      // r.w += 1;
      return {true, Rect{r.y, r.x, r.h, r.w + 1}, false};
    } else if (k == 2) {
      if (r.y <= 0 || !expandable)
        continue;
      // r.y -= 1;
      // r.h += 1;
      return {true, Rect{r.y - 1, r.x, r.h + 1, r.w}, false};
    } else if (k == 3) {
      if (r.y + r.h >= WMAX || !expandable)
        continue;
      // r.h += 1;
      return {true, Rect{r.y, r.x, r.h + 1, r.w}, false};
    } else if (k == 4) {
      // shrink の実装
      // その場合、ampをアスペクト比より大きな値にする
      // 矩形の形を変えられるようにする為。
      amp = (r.w + r.h - 1) / r.h;
      if (r.w <= amp || r.x + r.w - amp <= query.x)
        continue;
      // r.w -= amp;
      // never_hit = true;
      return {true, Rect{r.y, r.x, r.h, r.w - amp}, true};
    } else if (k == 5) {
      amp = (r.w + r.h - 1) / r.h;
      if (r.w <= amp || query.x <= r.x + (amp - 1))
        continue;
      // r.x += amp;
      // r.w -= amp;
      // never_hit = true;
      return {true, Rect{r.y, r.x + amp, r.h, r.w - amp}, true};
    } else if (k == 6) {
      amp = (r.h + r.w - 1) / r.w;
      if (r.h <= amp || r.y + r.h - amp <= query.y)
        continue;
      // r.h -= amp;
      // never_hit = true;
      return {true, Rect{r.y, r.x, r.h - amp, r.w}, true};
    } else if (k == 7) {
      amp = (r.h + r.w - 1) / r.w;
      if (r.h <= amp || query.y <= r.y + (amp - 1))
        continue;
      // r.y += amp;
      // r.h -= amp;
      // never_hit = true;
      return {true, Rect{r.y + amp, r.x, r.h - amp, r.w}, true};
    } else if (k == 8) {
      // 移動の実装
      if (r.x <= (amp - 1) || r.x + r.w - amp <= query.x)
        continue;
      // r.x -= amp;
      return {true, Rect{r.y, r.x - amp, r.h, r.w}, false};
    } else if (k == 9) {
      if (r.x + r.w >= WMAX - amp || query.x <= r.x + (amp - 1))
        continue;
      // r.x += amp;
      return {true, Rect{r.y, r.x + amp, r.h, r.w}, false};
    } else if (k == 10) {
      if (r.y <= (amp - 1) || r.y + r.h - amp <= query.y)
        continue;
      // r.y -= amp;
      return {true, Rect{r.y - amp, r.x, r.h, r.w}, false};
    } else if (k == 11) {
      if (r.y + r.h >= WMAX - amp || query.y <= r.y + (amp - 1))
        continue;
      // r.y += amp;
      return {true, Rect{r.y + amp, r.x, r.h, r.w}, false};
    }
  }
  return ResultMoveAdjacents{false, Rect{}, false};
}

double eliminateRects(vector<Rect>& rects,
                      const vector<Query>& queries,
                      const vector<int>& indices) {
  double increase_score = 0;  // may be negative
  // if (!indices.empty()) {
  //   // int r = rand(0, int(idxs.size() - 1));
  //   // shuffle(all(idxs), randdev);
  //   // rrepeat(r, min<int>(idxs.size(), 5)) {
  //   //   int i = idxs[r];
  for (int i : indices) {
    increase_score -= AMPSCORE * Algo::calcScore(rects[i].area(), queries[i].r);
    rects[i].x = queries[i].x;
    rects[i].y = queries[i].y;
    rects[i].w = 1;
    rects[i].h = 1;
    increase_score += AMPSCORE * Algo::calcScore(1, queries[i].r);
  }
  // }
  return increase_score;
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

  repeat(lop, 1500) {
    int vi = rand(0, int(targets.size()) - 1);
    int i = targets[vi];
    const auto& query = queries_[i];
    const auto& r = rects_[i];
    const bool expandable = query.r >= r.w * (r.h + 1) || query.r >= (r.w + 1) * r.h;
    if (!expandable) {
      targets.erase(targets.begin() + vi);
      continue;
    }
    repeat(tryk, 9) {  // expand only
      auto rect_old = rects_[i];
      auto r = rects_[i];
      int amp = 1;
      repeat(tryc, 9) {
        int k = rand(0, 3);
        amp = rand(1, std::max(100 / (tryc + 1), 5));
        if (k == 0) {
          if (r.x <= amp - 1)
            continue;
          r.x -= amp;
          r.w += amp;
          break;
        } else if (k == 1) {
          if (r.x + r.w >= WMAX - amp + 1)
            continue;
          r.w += amp;
          break;
        } else if (k == 2) {
          if (r.y <= amp - 1)
            continue;
          r.y -= amp;
          r.h += amp;
          break;
        } else if (k == 3) {
          if (r.y + r.h >= WMAX - amp + 1)
            continue;
          r.h += amp;
          break;
        }
      }
      rects_[i] = {-8, -8, 1, 1};
      if (Algo::checkHitIndex(rects_, r) >= 0) {
        rects_[i] = rect_old;
      } else {
        rects_[i] = r;
      }
    }
  }
}

void Solver::solve20(const Timer<>& timer) {
  constexpr int MaxTime = 5000;
  const int N = queries_.size();
  int tick_score_dump = 2;

  double current_score = Algo::calcScore(queries_, rects_);
  double best_score = current_score;
  vector<Rect> best_snap = rects_;
  int best_life = 100;

  // 既に接触したことのあるIDの列
  vector<vector<int>> hitting_history(N);

  int lop = 0;
  int time = 0;
  while ((lop & 127) || (time = timer.toc()) < MaxTime - 10) {
    int i = rand(0, N - 1);
    const auto& query = queries_[i];
    auto rect_old = rects_[i];

    auto adjres = Algo::moveAdjacents(rects_[i], query, double(time) / MaxTime);
    ++lop;
    if (!adjres.changed)
      continue;

    rects_[i] = {-99, -99, 1, 1};
    if (!adjres.never_hit && Algo::cachedCheckHit(rects_, adjres.act, hitting_history[i])) {
      // NG
      rects_[i] = rect_old;
    } else {
      // OK
      const auto& r = adjres.act;
      if (rect_old.area() != r.area()) {
        current_score -= AMPSCORE * Algo::calcScore(rect_old.area(), query.r);
        current_score += AMPSCORE * Algo::calcScore(r.area(), query.r);
      }
      rects_[i] = r;
    }

    // if (lop % 10 == 0) {
    {
      // 差分計算をする
      if (best_score < current_score) {
        chmax(best_life, 20);
        best_score = current_score;
        best_snap = rects_;
      } else {
        if (--best_life <= 0) {
          best_life = 20;  // 20ぐらいが良さげ
          rects_ = best_snap;
          current_score = best_score;
        }
      }
      if (!(lop & 1023) && tick_score_dump < timer.toc() / 100) {
        tick_score_dump++;
        clog << best_score << '\n';
      }
    }
    if (lop % 10000 == 0) {
      // 沙汰の実装。
      // 基準値を設けておき、それに届かないものを消す。
      // 1つ当たりどんなにひどくでも0点、良くても1点
      // 1つぐらい0にしたほうがマシなケースがあるのではと勝手に推測
      auto idxs = Algo::raiseWeakRect(queries_, rects_);
      current_score += Algo::eliminateRects(rects_, queries_, idxs);
      best_life = 50;
    }
    // verify score
    // if (abs(current_score - Algo::calcScore(queries_, rects_)) > 1e-1) {
    //   cerr << "error: score calculation " << current_score << " "
    //        << Algo::calcScore(queries_, rects_) << endl;
    //   abort();
    // }
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

  // 合議制を取る
  {
    double best_score = -1;
    decltype(rects_) best_snap;
    repeat(_, 300) {
      solve00(timer);
      solve10(timer);
      double score = Algo::calcScore(queries_, rects_);  // TODO: 同じ評価基準で良いのか？
      // 複雑にする理由はないし、以下は使わない
      // double score = 0;
      // repeat(i, (int)queries_.size()) {
      //   double x = Algo::calcScore(queries_[i].r, rects_[i].area());
      //   score += x > 0.4 ? x : 0;
      // } 28327283
      if (best_score < score) {
        best_snap = rects_;
        best_score = score;
      }
    }
    clog << "solve00-10 time: " << timer.toc() << " score: " << best_score << endl;
    rects_ = std::move(best_snap);
  }

  solve20(timer);
}

// ----------------------------------------------------------------------------

int main(int argc, char** argv) {
  Param::applyFromCommandLine(argc, argv);

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
