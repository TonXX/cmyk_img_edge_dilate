from PIL import Image, ImageFilter
import numpy as np
import sys
import gc


def get_edge_img(img_name):
    gc.collect()
    c_src_img = Image.open("./cmyk/c_"+img_name+".jpg")
    k_src_img = Image.open("./cmyk/k_"+img_name+".jpg")
    m_src_img = Image.open("./cmyk/m_"+img_name+".jpg")
    y_src_img = Image.open("./cmyk/y_"+img_name+".jpg")
    row, col = c_src_img.size
    c_src_arr = np.asarray(c_src_img.convert('L'))
    k_src_arr = np.asarray(k_src_img.convert('L'))
    m_src_arr = np.asarray(m_src_img.convert('L'))
    y_src_arr = np.asarray(y_src_img.convert('L'))
    # c_src_arr = np.asarray(c_src_img)
    # k_src_arr = np.asarray(k_src_img)
    # m_src_arr = np.asarray(m_src_img)
    # y_src_arr = np.asarray(y_src_img)
    del c_src_img, k_src_img, m_src_img, y_src_img
    cmyk_img_temp = Image.new('CMYK', (row, col), (0, 0, 0, 0))
    cmyk_arr = np.asarray(cmyk_img_temp)
    del cmyk_img_temp
    cmyk = cmyk_arr.copy()
    cmyk[:, :, 0] = c_src_arr
    cmyk[:, :, 1] = m_src_arr
    cmyk[:, :, 2] = y_src_arr
    cmyk[:, :, 3] = k_src_arr
    del cmyk_arr
    cmyk_img = Image.fromarray(cmyk, mode='CMYK')
    del cmyk
    cmyk_img.save("./cmyk/cmyk_ori.jpg")
    dilation_img = cmyk_img.filter(ImageFilter.MaxFilter(7))
    del cmyk_img
    # dilation_img.save("./cmyk/dilation_img.jpg")
    # erosion_img = cmyk_img.filter(ImageFilter.MinFilter(5))
    # erosion_img.save("./cmyk/erosion_img.jpg")
    blur_img = dilation_img.filter(ImageFilter.BLUR)
    del dilation_img
    # blur_img.save("./cmyk/blur_img.jpg")

    blur_arr = np.asarray(blur_img)
    # del blur_img
    C_blur = blur_arr[:, :, 0]
    M_blur = blur_arr[:, :, 1]
    Y_blur = blur_arr[:, :, 2]
    K_blur = blur_arr[:, :, 3]
    Image.fromarray(C_blur).save("./cmyk/c_blur.jpg")
    Image.fromarray(M_blur).save("./cmyk/m_blur.jpg")
    Image.fromarray(Y_blur).save("./cmyk/y_blur.jpg")
    Image.fromarray(K_blur).save("./cmyk/k_blur.jpg")
    return


if __name__ == '__main__':
    in_name = sys.argv[1]
    # print(in_name + " " + out_name)

    get_edge_img(in_name)
