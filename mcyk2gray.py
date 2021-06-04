from PIL import Image
import numpy as np
import sys
import gc


def add_edge(src_img_name):
    src_img = Image.open(src_img_name)
    if src_img.mode == "CMYK":
        row, col = src_img.size
        src_arr = np.asarray(src_img)
        del src_img
        C_src = src_arr[:, :, 0]
        M_src = src_arr[:, :, 1]
        Y_src = src_arr[:, :, 2]
        K_src = src_arr[:, :, 3]
        Image.fromarray(C_src).save("./cmyk/c_src.jpg")
        Image.fromarray(M_src).save("./cmyk/m_src.jpg")
        Image.fromarray(Y_src).save("./cmyk/y_src.jpg")
        Image.fromarray(K_src).save("./cmyk/k_src.jpg")
        del src_arr
        del C_src, M_src, Y_src, K_src
        return 0
    else:
        print("input img is not cmyk mode !!!!error")
        return -1


if __name__ == '__main__':
    in_name = sys.argv[1]
    add_edge(in_name)
