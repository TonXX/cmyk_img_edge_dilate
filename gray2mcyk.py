from PIL import Image
import numpy as np
import sys
import gc


def mix_img(name):
    gc.collect()
    c_src_img = Image.open("./cmyk/c_src_result.jpg")
    k_src_img = Image.open("./cmyk/k_src_result.jpg")
    m_src_img = Image.open("./cmyk/m_src_result.jpg")
    y_src_img = Image.open("./cmyk/y_src_result.jpg")
    row, col = c_src_img.size
    c_src_img = c_src_img.convert('L')
    k_src_img = k_src_img.convert('L')
    m_src_img = m_src_img.convert('L')
    y_src_img = y_src_img.convert('L')
    c_src_arr = np.asarray(c_src_img)
    k_src_arr = np.asarray(k_src_img)
    m_src_arr = np.asarray(m_src_img)
    y_src_arr = np.asarray(y_src_img)
    del c_src_img, k_src_img, m_src_img, y_src_img
    cmyk_img = Image.new('CMYK', (row, col), (0, 0, 0, 0))
    cmyk_arr = np.asarray(cmyk_img)
    del cmyk_img
    cmyk = cmyk_arr.copy()
    cmyk[:, :, 0] = c_src_arr
    cmyk[:, :, 1] = m_src_arr
    cmyk[:, :, 2] = y_src_arr
    cmyk[:, :, 3] = k_src_arr

    jpg_name = name + '.jpg'
    Image.fromarray(cmyk, mode='CMYK').save(jpg_name)

    return


if __name__ == '__main__':
    out_name = sys.argv[1]
    # print(in_name + " " + out_name)

    mix_img(out_name)
