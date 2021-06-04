# cmyk_img_edge_dilate


图像处理类型：cmyk图像
涉及到python c++ opencv PIL等


步骤：
1、生成Python exe程序。
2、运行c++代码


难点：
1、由于opencv没有找到处理cmyk图像的方法，所以引入了python PIL库进行处理。opencv对cmyk图像进行处理时，感觉会默认转换为rgb格式，导致图像产生色差。python PIL中的convert方法 rgb 和cmyk互转都会存在色差。
2、由于python 处理速度较慢，如果都使用Python处理效率极差。所以能用c++代码实现的我都迂回的方式通过c++实现了，实在必须pyton的才使用python处理、



效果展示：
命令： .\add_edge.exe 20 .\8.jpg 1 out
原图：
![8](https://user-images.githubusercontent.com/23007538/120733146-26c17900-c519-11eb-9cc0-91bbaf28db64.jpg)
外轮廓图：
![dilate](https://user-images.githubusercontent.com/23007538/120733244-5e302580-c519-11eb-9659-9ed44e93b5ec.jpg)
外轮廓cmyk图：
![cmyk_ori](https://user-images.githubusercontent.com/23007538/120733263-6ab47e00-c519-11eb-885d-9eba2afcc1d7.jpg)
合成后最终结果图：
![out](https://user-images.githubusercontent.com/23007538/120733296-7738d680-c519-11eb-8579-e0be564ce8c5.jpg)
