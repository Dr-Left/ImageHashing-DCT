#undef __STRICT_ANSI__
#include <cmath>

#include "my_io.cpp"

void DCT(int data[N][N], double output[N][N], pair<int, int> size) {
    int h = size.first, w = size.second;
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            double sum = 0;
            for (int x = 0; x < h; x++) {
                for (int y = 0; y < w; y++)
                    sum += data[x][y] *
                           cos((2 * x + 1) * u * M_PI / (2 * h)) *
                           cos((2 * y + 1) * v * M_PI / (2 * w));
            }
            sum *= sqrt(2.0 / h) * sqrt(2.0 / w);
            if (u == 0) sum *= 1 / sqrt(2.0);
            if (v == 0) sum *= 1 / sqrt(2.0);
            output[u][v] = sum;
        }
    }
}