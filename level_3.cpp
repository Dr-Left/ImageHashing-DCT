/*
perceptual hash algorithm
*/

#include <getopt.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int MODP = 10000007;
const string DEAFULT_DIR_PATH = "data_basic2";
const int MAX_QUERIES = 7;
const int MAXF = 10;  // max frequency matrix size
const int N = 512;
const int M = 64;

int hash_table[MODP];
int f_size = 6;  // frequency matrix size
string dir_path = DEAFULT_DIR_PATH;

uint8_t source_freq[30000000];

int build_hash_table();
void read_query_dst(uint8_t freq[MAXF][MAXF], string file_name);
pair<int, int> query(uint8_t query_freq_bin[MAXF][MAXF]);

int main(int argc, char *argv[]) {
    char ch;

    while ((ch = getopt(argc, argv, "D:F:")) != -1) switch (ch) {
            case 'D':
                dir_path = optarg;
                break;
            case 'F':
                f_size = atoi(optarg);
                break;
            case '?':
                if (optopt == 'D' || optopt == 'F')
                    fprintf(stderr, "Option -%c requires an argument.\n",
                            optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                abort();
        }
    ofstream ofs;

    ofs.open(dir_path + "/my_result.txt", ios::out);
    // timing
    auto start_time = chrono::high_resolution_clock::now();
    int cnt_conflict = build_hash_table();
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(
        end_time - start_time);
    cout << "Hash table created." << endl;
    cout << "    cnt_conflict: " << cnt_conflict << endl;
    cout << "    time: " << duration.count() << " microseconds" << endl;
    cout << endl;

    // query

    // timing
    start_time = chrono::high_resolution_clock::now();

    for (int i = 0; i <= MAX_QUERIES; i++) {
        uint8_t query_freq_bin[MAXF][MAXF];

        read_query_dst(query_freq_bin,
                       dir_path + "/query" + to_string(i) + "_dst.data");

        pair<int, int> pos = query(query_freq_bin);
        if (pos.first == -1 && pos.second == -1)
            printf("query{%d} not found.\n", i);
        else
            printf("query{%d} at (%d, %d).\n", i, pos.second,
                   pos.first);  // x is the line, y is the column
        ofs << pos.second << " " << pos.first << endl;
    }
    ofs.close();

    end_time = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end_time -
                                                           start_time);
    cout << "Query done.";
    cout << " Average query time: " << duration.count() / 8
         << " microseconds" << endl;

    // compare result
    ifstream ifs1, ifs2;
    ifs1.open(dir_path + "/my_result.txt", ios::in);
    ifs2.open(dir_path + "/result.txt", ios::in);
    if (!ifs1.is_open() || !ifs2.is_open()) {
        cout << "Error opening result file. End result comparing" << endl;
        exit(1);
    }
    // calculate the total manhatten diffrence
    int total_diff = 0;
    for (int i = 0; i <= MAX_QUERIES; i++) {
        int x1, y1, x2, y2;
        ifs1 >> x1 >> y1;
        ifs2 >> x2 >> y2;
        total_diff += abs(x1 - x2) + abs(y1 - y2);
    }
    cout << "Total manhatten diffrence between my result and answer: "
         << total_diff << endl;

    return 0;
}

unsigned get_hash_value(uint8_t arr[MAXF][MAXF]) {
    unsigned hash_value = 0;
    for (int i = 0; i < f_size; i++) {
        for (int j = 0; j < f_size; j++)
            hash_value = (hash_value * 2 + arr[i][j]) % MODP;
    }
    return hash_value;
}

void read_query_dst(uint8_t freq[MAXF][MAXF], string file_name) {
    ifstream ifs;
    ifs.open(file_name, ios::binary);

    if (!ifs.is_open()) {
        cout << "Error opening file" << endl;
        exit(1);
    }

    for (int i = 0; i < MAXF; i++) {
        for (int j = 0; j < MAXF; j++) {
            ifs.read(reinterpret_cast<char *>(&freq[i][j]),
                     sizeof(uint8_t));
        }
    }

    // print freq matrix
    // printf("freq matrix: \n");
    // for (int i = 0; i < f_size; i++) {
    //     for (int j = 0; j < f_size; j++) {
    //         printf("%d ", freq[i][j]);
    //     }
    //     printf("\n");
    // }
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
    ifs.open(dir_path + "/source_dst.data", ios::binary);
    if (!ifs.is_open()) {
        cout << "Error opening source dst file" << endl;
        exit(1);
    }
    int file_size = get_file_size(ifs);
    cout << "source file size: " << file_size << endl;

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
            uint8_t bin_freq[MAXF][MAXF];
            for (int x = 0; x < f_size; x++) {
                for (int y = 0; y < f_size; y++) {
                    int pos_order = i * (N - M + 1) + j;
                    bin_freq[x][y] = source_freq[MAXF * MAXF * pos_order +
                                                 MAXF * x + y];
                }
            }

            unsigned hash_value = get_hash_value(bin_freq);
            if (hash_table[hash_value] != 0) {
                // deal with conflicts
                cnt_conflict++;
                // linear trial
                while (hash_table[hash_value] != 0) {
                    hash_value = (hash_value + 1) % MODP;
                }
            }
            hash_table[hash_value] = i * (N - M + 1) + j + 1;
            // print the first bin_freq
            // if (i == 0 && j == 0) {
            //     printf("the first bin_freq: \n");
            //     for (int x = 0; x < f_size; x++) {
            //         for (int y = 0; y < f_size; y++) {
            //             printf("%d ", bin_freq[x][y]);
            //         }
            //         printf("\n");
            //     }
            // }
        }
    }
    return cnt_conflict;
}

bool element_wise_cmp(uint8_t query_freq_bin[MAXF][MAXF],
                      pair<int, int> pos) {
    int i = pos.first;
    int j = pos.second;
    for (int x = 0; x < f_size; x++) {
        for (int y = 0; x < f_size; x++) {
            int pos_order = i * (N - M + 1) + j;
            if (query_freq_bin[x][y] !=
                source_freq[MAXF * MAXF * pos_order + MAXF * x + y]) {
                return false;
            }
        }
    }
    return true;
}

pair<int, int> query(uint8_t query_freq_bin[MAXF][MAXF]) {
    unsigned hash_value = get_hash_value(query_freq_bin);
    // printf("hash_value: %u\n", hash_value);
    while (hash_table[hash_value] != 0) {
        int x = (hash_table[hash_value] - 1) / (N - M + 1);
        int y = (hash_table[hash_value] - 1) % (N - M + 1);
        // printf("x: %d, y: %d\n", x, y);
        if (element_wise_cmp(query_freq_bin, make_pair(x, y)))
            return make_pair(x, y);
        hash_value = (hash_value + 1) % MODP;
    }
    return make_pair(-1, -1);
}
