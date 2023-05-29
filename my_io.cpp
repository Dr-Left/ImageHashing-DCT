#include <iostream>
#include <string>
using namespace std;

#define N 512
#define M 64

pair<int, int> read_from_file(int img[N][N], string fpath) {
    // read
    FILE* im_file;
    int h, w;
    im_file = fopen(fpath.c_str(), "rb");
    fread(&h, sizeof(int), 1, im_file);
    fread(&w, sizeof(int), 1, im_file);
    // cout<<h<<" "<<w<<endl;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            int t;
            fread(&t, sizeof(int), 1, im_file);
            img[r][c] = t;
        }
    }
    fclose(im_file);
    return make_pair(h, w);
}

void save_to_file(int img[N][N], string fpath, pair<int, int> size) {
    // write
    FILE* im_file;
    int h = size.first, w = size.second;
    im_file = fopen(fpath.c_str(), "wb");
    fwrite(&h, sizeof(int), 1, im_file);
    fwrite(&w, sizeof(int), 1, im_file);
    // cout<<h<<" "<<w<<endl;
    for (int r = 0; r < h; r++) {
        for (int c = 0; c < w; c++) {
            int t = img[r][c];
            fwrite(&t, sizeof(int), 1, im_file);
        }
    }
    fclose(im_file);
}

// int main()
// {
//     int img[512][512];
//     read_from_file(img, "./data_basic1/query0_g.data");
//     save_to_file(img, "./data_basic1/query0_g_copy.data");
//     return 0;
// }
