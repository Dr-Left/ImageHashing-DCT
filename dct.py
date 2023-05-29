from scipy.fftpack import dct, idct
from skimage.io import imread
from skimage.color import rgb2gray
import numpy as np
import argparse

DEFAULT_FREQ_MATRIX_SIZE = 6


# implement 2D DCT
def dct2(a):
    return dct(dct(a.T, norm="ortho").T, norm="ortho")


freq_matrix_size = 6


def calc_imf(im, fp, fp2):
    for i in range(0, 512 - 64 + 1):
        for j in range(0, 512 - 64 + 1):
            imF = dct2(im[i : i + 64, j : j + 64])
            # binarize the DCT result by average value
            binarized_imF = (imF > np.mean(imF)).astype(np.uint8)
            needed_data = binarized_imF[:freq_matrix_size, :freq_matrix_size]
            needed_data.tofile(fp)
            np.savetxt(fp2, needed_data, fmt="%d")  # txt ouput for debugging(readable)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--directory",
        "-D",
        type=str,
        required=True,
        help="The path of the image files",
    )
    parser.add_argument(
        "--frequency_matrix_size",
        "-F",
        type=int,
        default=DEFAULT_FREQ_MATRIX_SIZE,
        required=False,
        help="The size of the frequency matrix",
    )
    args = parser.parse_args()

    freq_matrix_size = args.frequency_matrix_size
    dir_path = args.directory

    # process query images
    for query_num in range(8):
        im = rgb2gray(imread(f"{dir_path}/query{query_num}.png"))
        imF = dct2(im)

        # binarize the DCT result by average value
        binarized_imF = (imF > np.mean(imF)).astype(np.uint8)[
            :freq_matrix_size, :freq_matrix_size
        ]

        with open(f"{dir_path}/query{query_num}_dst.data", "wb") as fp:
            binarized_imF.tofile(fp)
            np.savetxt(f"{dir_path}/query{query_num}_dst.txt", binarized_imF, fmt="%d")
        if query_num == 0:
            print(binarized_imF)

    # process source images
    im = rgb2gray(imread(f"{dir_path}/source.png"))

    with open(f"{dir_path}/source_dst.data", "wb") as fp_data:
        with open(f"{dir_path}/source_dst.txt", "w") as fp_txt:
            calc_imf(im, fp_data, fp_txt)
