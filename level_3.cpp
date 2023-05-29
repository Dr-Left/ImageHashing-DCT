/*
perceptual hash algorithm
*/

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int MODP = 10000007;
const string dir_path = "./data_basic2/";
const int MAX_QUERIES = 7;  // TODO: change this to 7
const int CNT_QUERY = 6;
const int N = 512;
const int M = 64;

int hash_table[MODP];

uint8_t source_freq[9000000];

int build_hash_table();
void read_query_dst(uint8_t freq[CNT_QUERY][CNT_QUERY], string file_name,
                    pair<int, int> size);
pair<int, int> query(uint8_t query_freq_bin[CNT_QUERY][CNT_QUERY],
                     pair<int, int> size);

int main() {
    ofstream ofs;
    ofs.open(dir_path + "my_result.txt", ios::out);

    int cnt_conflict = build_hash_table();
    cout << "cnt_conflict: " << cnt_conflict << endl;

    for (int i = 0; i <= MAX_QUERIES; i++) {
        uint8_t query_freq_bin[CNT_QUERY][CNT_QUERY];

        read_query_dst(query_freq_bin,
                       dir_path + "query" + to_string(i) + "_dst.data",
                       make_pair(CNT_QUERY, CNT_QUERY));

        pair<int, int> pos =
            query(query_freq_bin, make_pair(CNT_QUERY, CNT_QUERY));
        if (pos.first == -1 && pos.second == -1)
            printf("query{%d} not found.\n", i);
        else
            printf("query{%d} at (%d, %d).\n", i, pos.second,
                   pos.first);  // x is the line, y is the column
        ofs << pos.second << " " << pos.first << endl;
    }
    return 0;
}

unsigned get_hash_value(uint8_t arr[CNT_QUERY][CNT_QUERY],
                        pair<int, int> size) {
    int h = size.first, w = size.second;
    unsigned hash_value = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            hash_value = (hash_value * 2 + arr[i][j]) % MODP;
    }
    return hash_value;
}

void read_query_dst(uint8_t freq[CNT_QUERY][CNT_QUERY], string file_name,
                    pair<int, int> size) {
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

int get_file_size(ifstream &ifs) {
    ifs.seekg(0, ios::end);
    int file_size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    return file_size;
}

void read_source_dst() {
    ifstream ifs;
    ifs.open(dir_path + "source_dst.data", ios::binary);
    if (!ifs.is_open()) {
        cout << "Error opening source dst file" << endl;
        exit(1);
    }
    int file_size = get_file_size(ifs);
    // cout << "source file size: " << file_size << endl;

    ifs.seekg(0, ios::beg);
    ifs.read(reinterpret_cast<char *>(source_freq), file_size);
}

int build_hash_table() {
    /*
        build hash table from source dst file
    */

    read_source_dst();

    int cnt_conflict = 0;
    for (int i = 0; i < N - M + 1; i++) {
        for (int j = 0; j < N - M + 1; j++) {
            uint8_t bin_freq[CNT_QUERY][CNT_QUERY];
            for (int x = 0; x < CNT_QUERY; x++) {
                for (int y = 0; y < CNT_QUERY; y++) {
                    bin_freq[x][y] =
                        source_freq[CNT_QUERY * CNT_QUERY *
                                        (i * (N - M + 1) + j) +
                                    CNT_QUERY * x + y];
                }
            }

            unsigned hash_value =
                get_hash_value(bin_freq, make_pair(CNT_QUERY, CNT_QUERY));
            if (hash_table[hash_value] != 0) cnt_conflict++;
            hash_table[hash_value] = i * (N - M + 1) + j + 1;
        }
    }
    return cnt_conflict;
}

pair<int, int> query(uint8_t query_freq_bin[CNT_QUERY][CNT_QUERY],
                     pair<int, int> size) {
    unsigned hash_value = get_hash_value(query_freq_bin, size);
    // printf("hash_value: %u\n", hash_value);
    if (hash_table[hash_value] != 0) {
        int x = (hash_table[hash_value] - 1) / (N - M + 1);
        int y = (hash_table[hash_value] - 1) % (N - M + 1);
        return make_pair(x, y);
    }
    return make_pair(-1, -1);
}
