/*
perceptual hash algorithm
*/

#include <fstream>

#include "dct.cpp"

const int MODP = 1000007;
const string dir_path = "./data_basic2/";
const int MAX_QUERIES = 7;  // TODO: change this to 7
const int QUERY_SIZE = 6;

int hash_table[MODP];

uint8_t source_freq[9000000];

int build_hash_table();
void read_query_dst(uint8_t freq[QUERY_SIZE][QUERY_SIZE],
                    string file_name, pair<int, int> size);
void binarize(double arr[QUERY_SIZE][QUERY_SIZE],
              int bin_arr[QUERY_SIZE][QUERY_SIZE], pair<int, int> size,
              double threshold);
unsigned get_hash_value(uint8_t arr[QUERY_SIZE][QUERY_SIZE],
                        pair<int, int> size);
pair<int, int> query(uint8_t query_freq_bin[QUERY_SIZE][QUERY_SIZE],
                     pair<int, int> size);

int main() {
    int cnt_conflict = build_hash_table();
    cout << "cnt_conflict: " << cnt_conflict << endl;
    // print hash value in 971348
    cout << "hash value of 971348: " << hash_table[971348] << endl;

    for (int i = 0; i <= MAX_QUERIES; i++) {
        uint8_t query_freq_bin[QUERY_SIZE][QUERY_SIZE];
        read_query_dst(query_freq_bin,
                       dir_path + "query" + to_string(i) + "_dst.data",
                       make_pair(QUERY_SIZE, QUERY_SIZE));

        // debugging: print query_freq_bin
        for (int i = 0; i < QUERY_SIZE; i++) {
            for (int j = 0; j < QUERY_SIZE; j++)
                printf("%d ", query_freq_bin[i][j]);
            printf("\n");
        }

        pair<int, int> pos =
            query(query_freq_bin, make_pair(QUERY_SIZE, QUERY_SIZE));
        if (pos.first == -1 && pos.second == -1)
            printf("query{%d} not found.\n", i);
        else
            printf("query{%d} at (%d, %d).\n", i, pos.second,
                   pos.first);  // x is the line, y is the column
    }
    return 0;
}

void read_query_dst(uint8_t freq[QUERY_SIZE][QUERY_SIZE],
                    string file_name, pair<int, int> size) {
    ifstream ifs;
    ifs.open(file_name, ios::binary);

    if (!ifs.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    int h = size.first, w = size.second;

    ifs.read(reinterpret_cast<char *>(freq), h * w * sizeof(uint8_t));
    ifs.close();
}

int build_hash_table() {
    ifstream ifs;
    ifs.open(dir_path + "source_dst.data", ios::binary);
    if (!ifs.is_open()) {
        cout << "Error opening source dst file" << endl;
        exit(1);
    }

    std::streampos begin, end;

    begin = ifs.tellg();
    ifs.seekg(0, ios::end);
    end = ifs.tellg();
    int size = end - begin;

    cout << "source file size: " << size << endl;

    ifs.seekg(0, ios::beg);
    ifs.read(reinterpret_cast<char *>(source_freq), size);

    // print the first 36 elements of source_freq
    // printf("source_freq:\n");
    // for (int i = 0; i < 36; i++) {
    //     printf("%d ", source_freq[i]);
    // }

    int cnt_conflict = 0;
    for (int i = 0; i < N - M + 1; i++) {
        for (int j = 0; j < N - M + 1; j++) {
            uint8_t bin_freq[QUERY_SIZE][QUERY_SIZE];
            for (int x = 0; x < QUERY_SIZE; x++) {
                for (int y = 0; y < QUERY_SIZE; y++) {
                    bin_freq[x][y] =
                        source_freq[QUERY_SIZE * QUERY_SIZE *
                                        (i * (N - M + 1) + j) +
                                    QUERY_SIZE * x + y];
                }
            }
            // print the first bin_freq
            // if (i == 0 && j == 0) {
            //     printf("bin_freq in source at (%d, %d):\n", i, j);
            //     for (int x = 0; x < QUERY_SIZE; x++) {
            //         for (int y = 0; y < QUERY_SIZE; y++) {
            //             printf("%hu ", bin_freq[x][y]);
            //         }
            //         printf("\n");
            //     }
            // }

            unsigned hash_value = get_hash_value(
                bin_freq, make_pair(QUERY_SIZE, QUERY_SIZE));
            if (hash_table[hash_value] != 0) cnt_conflict++;
            hash_table[hash_value] = i * (N - M + 1) + j + 1;
            if (i == 52 && j == 65)
                printf(
                    "hash_value in source at (%d, %d): "
                    "%u\n",
                    i, j, hash_value);
        }
    }
    return cnt_conflict;
}

pair<int, int> query(uint8_t query_freq_bin[QUERY_SIZE][QUERY_SIZE],
                     pair<int, int> size) {
    unsigned hash_value = get_hash_value(query_freq_bin, size);
    printf("hash_value: %u\n", hash_value);
    if (hash_table[hash_value] != 0) {
        int x = (hash_table[hash_value] - 1) / (N - M + 1);
        int y = (hash_table[hash_value] - 1) % (N - M + 1);
        return make_pair(x, y);
    }
    return make_pair(-1, -1);
}

void binarize(double arr[QUERY_SIZE][QUERY_SIZE],
              int bin_arr[QUERY_SIZE][QUERY_SIZE], pair<int, int> size,
              double threshold) {
    int h = size.first, w = size.second;
    if (threshold == 0) {
        // default: use average as threshold
        double sum = 0;
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) sum += arr[i][j];
        threshold = sum / (h * w);
        // printf("threshold: %lf\n", threshold);
    }
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            bin_arr[i][j] = (arr[i][j] > threshold);
}

unsigned get_hash_value(uint8_t arr[QUERY_SIZE][QUERY_SIZE],
                        pair<int, int> size) {
    int h = size.first, w = size.second;
    unsigned hash_value = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            hash_value = (hash_value * 2 + arr[i][j]) % MODP;
    }
    return hash_value;
}