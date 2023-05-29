import cv2
import numpy as np
import os
import argparse


parser = argparse.ArgumentParser()

parser.add_argument('--i', type=str, help='Input path.')
parser.add_argument('--rgb2g', action='store_true', help='Transform rgb image to gray image.')
parser.add_argument('--g2d', action='store_true', help='Transform gray image to binary file.')
parser.add_argument('--d2g', action='store_true', help='Transform binary data to gray image.')
parser.add_argument('--show', action='store_true', help='Show result')
args = parser.parse_args()

i_path = args.i

if args.rgb2g:
    if os.path.isdir(i_path):
        for f_name in os.listdir(i_path):
            if f_name.__len__() >= 4 and f_name[-4:] == '.png':
                img = cv2.imread(os.path.join(i_path, f_name), cv2.IMREAD_GRAYSCALE)
                cv2.imwrite(os.path.join(i_path, f_name[:-4] + '_g' + '.png'), img)
    else:
        f_name = i_path
        img = cv2.imread(f_name, cv2.IMREAD_GRAYSCALE)
        cv2.imwrite((f_name[:-4] + '_g' + '.png'), img)

elif args.g2d:
    if os.path.isdir(i_path):
        for f_name in os.listdir(i_path):
            if f_name.__len__() >= 6 and f_name[-6:] == '_g.png':
                img = cv2.imread(os.path.join(i_path, f_name), cv2.IMREAD_GRAYSCALE).astype(int)
                h, w = img.shape[0], img.shape[1]
               
                f = open(os.path.join(i_path, f_name[:-4] + '.data'), 'wb')
                f.write(h.to_bytes(4, 'little'))
                f.write(w.to_bytes(4, 'little'))
                for r in range(h):
                    for c in range(w):
                        t = int(img[r, c])
                        f.write(t.to_bytes(4, 'little'))
                f.close()
    else:
        f_name = i_path
        img = cv2.imread(f_name, cv2.IMREAD_GRAYSCALE).astype(int)
        h, w = img.shape[0], img.shape[1]
        f = open((f_name[:-4] + '.data'), 'wb')
        f.write(h.to_bytes(4, 'little'))
        f.write(w.to_bytes(4, 'little'))
        for r in range(h):
            for c in range(w):
                t = int(img[r, c])
                f.write(t.to_bytes(4, 'little'))
        f.close()
elif args.d2g:
    if os.path.isdir(i_path):
        for f_name in os.listdir(i_path):
            if f_name.__len__() >= 5 and f_name[-5:] == '.data':
                f = open(os.path.join(i_path, f_name), 'rb')
                h = int.from_bytes(f.read(4), 'little')
                w = int.from_bytes(f.read(4), 'little')
                img = np.zeros((h, w)).astype(int)
                for r in range(h):
                    for c in range(w):
                        img[r, c] = int.from_bytes(f.read(4), 'little')

                cv2.imwrite(os.path.join(i_path, f_name[:-5] + '.png'), img)
                f.close()
    else:
        f_name = i_path
        f = open((f_name), 'rb')
        h = int.from_bytes(f.read(4), 'little')
        w = int.from_bytes(f.read(4), 'little')
        img = np.zeros((h, w)).astype(int)
        for r in range(h):
            for c in range(w):
                img[r, c] = int.from_bytes(f.read(4), 'little')

        cv2.imwrite((f_name[:-5] + '.png'), img)
        f.close()

elif args.show:
    source = cv2.imread(os.path.join(i_path, 'source_g.png'))
    query_list = []
    for i in range(8):
        query = cv2.imread(os.path.join(i_path, 'query%d_g.png' % i))
        query_list.append(query)
    show_img = np.zeros((512, 512+64, 3)).astype(np.uint8)
    show_img[:512, :512, :3] = source
    for i in range(8):
        show_img[i*64:(i+1)*64, 512:, :3] = query_list[i]
    f = open(os.path.join(i_path, 'result.txt'), 'r')
    for i in range(8):
        x, y = f.readline().split(' ')
        x, y = int(x), int(y)
        color = ((i%2)*255, ((i//2)%2)*255, ((i//4)%2)*255)
        show_img = cv2.rectangle(show_img, (x, y), (x+64, y+64), color, 2)
        show_img = cv2.rectangle(show_img, (512, i*64), (512+64, i*64+64), color, 2)
    f.close()
    cv2.imshow('show_img', show_img)
    cv2.waitKey()


    