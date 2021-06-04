from PIL import Image
import numpy as np
import sys
import gc


def add_edge(src_img_name, edge_img_name):
    gc.collect()
    edge_cmyk_img = Image.open(edge_img_name)
    if edge_cmyk_img.mode != "CMYK":
        edge_cmyk_img = edge_cmyk_img.convert('CMYK')

    edge_arr = np.asarray(edge_cmyk_img)
    del edge_cmyk_img
    C_edge = edge_arr[:, :, 0]
    M_edge = edge_arr[:, :, 1]
    Y_edge = edge_arr[:, :, 2]
    K_edge = edge_arr[:, :, 3]
    del edge_arr

    Image.fromarray(C_edge).save("./cmyk/c_edge.jpg")
    Image.fromarray(M_edge).save("./cmyk/m_edge.jpg")
    Image.fromarray(Y_edge).save("./cmyk/y_edge.jpg")
    Image.fromarray(K_edge).save("./cmyk/k_edge.jpg")
    del C_edge, M_edge, Y_edge, K_edge
    gc.collect()

    src_img = Image.open(src_img_name)
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
    # gc.collect()
    # for x in range(0,  row):
    #     for y in range(0,  col):
    #         if C_edge[x][y] == 0 or M_edge[x][y] == 0 or Y_edge[x][y] == 0 or K_edge[x][y] == 0:
    #             continue
    #         else:
    #             C_src[x][y] == C_edge[x][y]
    #             M_src[x][y] == M_edge[x][y]
    #             Y_src[x][y] == Y_edge[x][y]
    #             K_src[x][y] == K_edge[x][y]

    # src_img[:, :, 0] = C_src
    # src_img[:, :, 1] = M_src
    # src_img[:, :, 2] = Y_src
    # src_img[:, :, 3] = K_src

    # Image.fromarray(src_img, mode='CMYK').save("./cmyk/cmyk.jpg")

    return


if __name__ == '__main__':
    in_name = sys.argv[1]
    # out_name = sys.argv[2]
    # print(in_name + " " + out_name)


    mix_img = add_edge(in_name, "edge.jpg")

    #row, col = img.size

# for x in range(0,  row):
#    for y in range(0,  col):
# cmyk[:, :, 0] = 255 - C
# cmyk[:, :, 1] = 255 - M
# cmyk[:, :, 2] = 255 - Y
# cmyk[:, :, 3] = 255 - K

# Image.fromarray(cmyk, mode='CMYK').save("./cmyk/cmyk.jpg")
