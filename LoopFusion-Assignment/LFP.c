#include <stdio.h>
#include <time.h>

void foo(int N, int* a, int* b, int* c, int* d) {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = 1 / b[i] * c[i];
  }

  for (i = 0; i < N; i++) {
    d[i] = a[i] + c[i];
  }
}

int main() {
    int N = 30;
    int a[N], b[N], c[N], d[N];
    
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
      a[i] = rand();
      b[i] = rand();
      c[i] = rand();
      d[i] = 0;
    }
    foo(N, a, b, c, d);
  return 0;
}
