# 图像哈希 Image Hashing
level_1使用直接hash的方式。Direct hashing
level_3使用感知哈希的方式。Perceptual hashing

## Usage
### Run dct.py to generate DCT files.
```bash
python dct.py -D data_basic2 -F 10
```
-D means the directory path, -F means the frequency matrix size we needed from the dct result. (The C++ program now stick to 10, so use python always with 10)

### Then Run level_3.cpp
```bash
level_3.exe -D data_basic2 -F 5
```
Here the Cpp program can flexibly use the first F * F part of the dct matrix.
Then use hash method to find the place for 8 queries.

The program will do time counting, hash conflict counting, result comparing(with the correct answer in data_basic1 and data_basic2)

## Insights
Can use this project to compare the influnce of different matrix size on the performance.
