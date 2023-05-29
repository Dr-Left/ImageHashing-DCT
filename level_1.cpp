#include <chrono>

#include "my_io.cpp"

const string dir_path = "./data_basic1/";
const int MODP = 100000007;
const int MAX_QUERIES = 7;

int hash_table[MODP];

int img[N][N];

int build_hash_table(int img[N][N], int hash_table[M],
                     pair<int, int> size) {
    int cnt_conflict = 0;
    int h = size.first, w = size.second;
    for (int x = 0; x <= h - M; x++) {
        for (int y = 0; y <= w - M; y++) {
            unsigned hash_value = 0;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++)
                    hash_value =
                        (hash_value * 256 + img[x + i][y + j]) % MODP;
            }
            if (hash_table[hash_value] != 0) cnt_conflict++;
            hash_table[hash_value] = x * N + y + 1;
        }
    }
    return cnt_conflict;
}

pair<int, int> query(int query_img[N][N], pair<int, int> size) {
    int h = size.first, w = size.second;
    for (int x = 0; x <= h - M; x++) {
        for (int y = 0; y <= w - M; y++) {
            unsigned hash_value = 0;
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < M; j++)
                    hash_value =
                        (hash_value * 256 + query_img[x + i][y + j]) %
                        MODP;
            }
            if (hash_table[hash_value] != 0) {
                int x0 = (hash_table[hash_value] - 1) / N;
                int y0 = (hash_table[hash_value] - 1) % N;
                return make_pair(x0, y0);
            }
        }
    }
    return make_pair(-1, -1);
}

int main() {
    // printf("%llu", sizeof(hash_table) / 1024 / 1024);
    pair<int, int> size = read_from_file(img, dir_path + "source_g.data");

    auto start_time = chrono::high_resolution_clock::now();

    int cnt_conflict = build_hash_table(img, hash_table, size);

    auto end_time = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(
        end_time - start_time);

    printf("Hash table created.\n");
    printf("    time: %lld ms\n", duration.count());
    printf("    conflicts: %d\n", cnt_conflict);

    FILE* my_result_file;
    my_result_file = fopen((dir_path + "my_result.txt").c_str(), "w");

    for (int i = 0; i <= MAX_QUERIES; i++) {
        int query_img[N][N];
        pair<int, int> size = read_from_file(
            query_img, dir_path + "query" + to_string(i) + "_g.data");
        pair<int, int> pos = query(query_img, size);
        if (pos.first == -1 && pos.second == -1)
            printf("query{%d} not found.\n", i);
        else
            printf("query{%d} at (%d, %d).\n", i, pos.second,
                   pos.first);  // x is the line, y is the column
        fprintf(my_result_file, "%d %d\n", pos.second, pos.first);
    }

    // save_to_file(img, dir_path + "query0_g_copy.data", size);
    return 0;
}