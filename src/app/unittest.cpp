#include "header.hpp"
#include "basic.hpp"

#include <gtest/gtest.h>

TEST(RectTest, Hitting) {
  constexpr int N = 7;
  bool field[N][N];
  repeat(y1, N) {
    iterate(h1, 1, N - y1) {
      repeat(x1, N) {
        iterate(w1, 1, N - x1) {
          fill(field[0], field[N], 0);
          iterate(x, x1, x1 + w1) {
            iterate(y, y1, y1 + h1) { field[y][x] = 1; }
          }
          Rect r1{y1, x1, h1, w1};
          repeat(y2, N) {
            iterate(h2, 1, N - y2) {
              repeat(x2, N) {
                iterate(w2, 1, N - x2) {
                  Rect r2{y2, x2, h2, w2};
                  bool hit = false;
                  iterate(x, x2, x2 + w2) {
                    iterate(y, y2, y2 + h2) { hit |= field[y][x]; }
                  }
                  EXPECT_EQ(hit, r1.crashTo(r2)) << r1 << "/" << r2;
                  if (hit != r1.crashTo(r2))
                    return;
                }
              }
            }
          }
        }
      }
    }
  }
}
