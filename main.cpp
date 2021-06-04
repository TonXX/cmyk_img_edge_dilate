#include<iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
//-----------------------------------【全局函数声明部分】-----------------------------------
//          描述：全局函数声明
//-----------------------------------------------------------------------------------------------
void colorReduce(Mat& inputImage, Mat& outputImage, int div);
void color_mix(Mat& srcImage, Mat& dilateImage, Mat& outputImage, int add_width);
void color_mix_cmyk(Mat& srcImage, Mat& dilateImage,  int add_width);
void find_insert_point(Mat &src_img, cv::Point &for_point, cv::Point &out_point, int add_width);
void creat_real_color_canny_edge(Mat &src_img, Mat &canny_img, Mat &dst_img);
void to_black_img(Mat &in, Mat &out);
void add_img(Mat &src, Mat &edge, Mat &out);
void add_python_img();
void find_contours(Mat &in, Mat &out, int mode);
void ShowHelpText();

static void paintAlphaMat(Mat &mat)
{
    CV_Assert(mat.channels() == 4);
    for (int i = 0; i < mat.rows; ++i)
    {
        for (int j = 0; j < mat.cols; ++j)
        {
            Vec4b& bgra = mat.at<Vec4b>(i, j);
            bgra[0] = UCHAR_MAX; // Blue
            bgra[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX); // Green
            bgra[2] = saturate_cast<uchar>((float(mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX); // Red
            bgra[3] = saturate_cast<uchar>(0.5 * (bgra[1] + bgra[2])); // Alpha
        }
    }
}
//--------------------------------------【main( )函数】---------------------------------------
//          描述：控制台应用程序的入口函数，我们的程序从这里开始执行
//-----------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    std::cout << "running" << std::endl;
    if (argc != 4 && argc != 5)
    {
        std::cout << "error input" << std::endl;
        return -1;
    }
    string add_edge_width_str = argv[1];
    string img_path = argv[2];
    string mode = argv[3];//1 外扩，2，内敛
    string out_name;
    if (argc == 5)
        out_name = argv[4];
    else
    {
        out_name = "new";
    }
    int mode_int = atoi(mode.c_str());
    int add_edge_width = atoi(add_edge_width_str.c_str());

    std::cout << "add_edge_width = " << add_edge_width << std::endl;
    std::cout << "img_path = " << img_path << std::endl;

    //cv::Mat img = imread(img_path, IMREAD_GRAYSCALE);

    //【1】创建原始图并显示
    Mat srcImage = imread(img_path);
    //srcImage.convertTo()
    std::cout << "read ok" << std::endl;
    //cv::resize(srcImage, srcImage, Size(srcImage.cols / 2, srcImage.rows / 2));
    Mat canny_Image;
    //imshow("old",srcImage);  
    //Mat black_img;
    //to_black_img(srcImage, black_img);
    //【2】按原始图的参数规格来创建创建效果图
    Mat dstImage, grayImage, dilate_img, result_img, add_result_img, real_color_canny_edge_img;
    //dstImage.create(srcImage.rows + 2*add_edge_width, srcImage.cols + 2*add_edge_width, srcImage.type());//效果图的大小、类型与原图片相同 

    //检测边缘
    //cv::cvtColor(srcImage, grayImage, CV_BGR2GRAY);
    to_black_img(srcImage, grayImage);
    //cv::imwrite("./out/grayImage.jpg", grayImage);
    threshold(grayImage, grayImage, 250, 255, THRESH_OTSU);
    bitwise_not(grayImage, grayImage);
    std::cout << "threshold ok" << std::endl;
    //cv::imwrite("./out/threshold.jpg", grayImage);
    GaussianBlur(grayImage, grayImage, Size(3, 3), 1.5, 1.5);
    //Canny(grayImage, canny_Image, 100, 250, 3);
    find_contours(grayImage, canny_Image, mode_int);

    //creat_real_color_canny_edge(srcImage, canny_Image, real_color_canny_edge_img);

    // imshow("canny_Image", canny_Image);
    //cv::imwrite("./out/contours.jpg", canny_Image);
    std::cout << "find contours ok" << std::endl;
    //膨胀边缘
    Mat element = getStructuringElement(MORPH_CROSS, Size(add_edge_width, add_edge_width));
    dilate(canny_Image, dilate_img, element, cv::Point(-1, -1), 1, 2);
    cv::imwrite("./out/dilate.jpg", dilate_img);
    std::cout << "dilate ok" << std::endl;

    //生成mcyk四通道灰度图
    string mcyk2gray = "mcyk2gray.exe " + img_path;
    system(mcyk2gray.c_str());
    std::cout << "call python exe mcyk2gray ok " << std::endl;

    //imwrite("dilate_img.jpg", dilate_img);
    //边缘填充融合
    //color_mix(srcImage, dilate_img, result_img, add_edge_width);

    //根据原图mcyk四通道灰度图和边界二值图获取  边界四通道灰度图
    color_mix_cmyk(srcImage, dilate_img, add_edge_width);
    std::cout << "gen edge ok" << std::endl;

    //根据边界四通道灰度图生成边界mcyk图。
    string gray2mcyk_ex = "gray2mcyk_ex.exe  edge";
    system(gray2mcyk_ex.c_str());
    std::cout << "call python exe gray2mcyk_ex ok " << std::endl;
    //Mat element_close = getStructuringElement(MORPH_RECT, Size(add_edge_width/2, add_edge_width/2));
    //morphologyEx(result_img, result_img, MORPH_OPEN, element_close);
    //bilateralFilter(result_img, result_img, 9, 75, 75);
    int medianBlur_size = add_edge_width;
    if (add_edge_width > 5)
    {
        if (medianBlur_size % 2 == 0 && medianBlur_size > 5)
            //偶数
            medianBlur_size = medianBlur_size - 3;
        else
            //奇数
            medianBlur_size = medianBlur_size - 4;
    }
    else
        medianBlur_size = 5;
/*
    Mat element2 = getStructuringElement(MORPH_CROSS, Size(add_edge_width, add_edge_width));
    erode(result_img, result_img, element2, cv::Point(-1, -1), 1, 2);
    std::cout << "erode ok " << std::endl;
    medianBlur(result_img, result_img, 7);
    GaussianBlur(result_img, result_img, Size(5, 5), 1.5, 1.5);
    std::cout << "medianBlur ok " << std::endl;
*/
    //imwrite("edge.jpg", result_img);
    //add_img(srcImage, result_img, add_result_img);
    //std::string output_name = out_name + ".jpg";
    //imwrite(output_name.c_str(), add_result_img);
    //std::cout << out_name << "ok" << std::endl;
/*
    string commond = "mcyk2rgb.exe " + img_path ;
    system(commond.c_str());
    std::cout << "call python exe ok "<< std::endl;
*/
    add_python_img();

    string commond2 = "gray2mcyk.exe " + out_name;
    system(commond2.c_str());
    std::cout << "call python exe to mix ok " << std::endl;

    return 0;
}
static int my_count = 0;

void add_python_img()
{
    Mat c_src_img = imread("./cmyk/c_src.jpg",0);
    Mat k_src_img = imread("./cmyk/k_src.jpg", 0);
    Mat m_src_img = imread("./cmyk/m_src.jpg", 0);
    Mat y_src_img = imread("./cmyk/y_src.jpg", 0);

    Mat c_edge_img = imread("./cmyk/c_blur.jpg", 0);
    Mat k_edge_img = imread("./cmyk/k_blur.jpg", 0);
    Mat m_edge_img = imread("./cmyk/m_blur.jpg", 0);
    Mat y_edge_img = imread("./cmyk/y_blur.jpg", 0);

    int rowNumber = c_src_img.rows;  //行数
    int colNumber = c_src_img.cols;  //列数 x 通道数=每一行元素的个数
    //in.copyTo(out);
    for (int i = 0; i < rowNumber; i++)
    {
        uchar* c_src_data = c_src_img.ptr<uchar>(i);
        uchar* k_src_data = k_src_img.ptr<uchar>(i);
        uchar* m_src_data = m_src_img.ptr<uchar>(i);
        uchar* y_src_data = y_src_img.ptr<uchar>(i);

        uchar* c_edge_img_data = c_edge_img.ptr<uchar>(i);
        uchar* k_edge_img_data = k_edge_img.ptr<uchar>(i);
        uchar* m_edge_img_data = m_edge_img.ptr<uchar>(i);
        uchar* y_edge_img_data = y_edge_img.ptr<uchar>(i);

        for (int j = 0; j < colNumber; j++)
        {
            if ((
                c_src_data[j] != 0 ||
                k_src_data[j] != 0 ||
                m_src_data[j] != 0 ||
                y_src_data[j] != 0
                ) && (
                c_edge_img_data[j] == 0 &&
                k_edge_img_data[j] == 0 &&
                m_edge_img_data[j] == 0 &&
                y_edge_img_data[j] == 0))
            {
                c_src_data[j] = c_src_data[j];
                k_src_data[j] = k_src_data[j];
                m_src_data[j] = m_src_data[j];
                y_src_data[j] = y_src_data[j];
            }
            else if ((
                c_edge_img_data[j] != 0 ||
                k_edge_img_data[j] != 0 ||
                m_edge_img_data[j] != 0 ||
                y_edge_img_data[j] != 0
                ) && (
                c_src_data[j] == 0 &&
                k_src_data[j] == 0 &&
                m_src_data[j] == 0 &&
                y_src_data[j] == 0))

            {
                c_src_data[j] = c_edge_img_data[j];
                k_src_data[j] = k_edge_img_data[j];
                m_src_data[j] = m_edge_img_data[j];
                y_src_data[j] = y_edge_img_data[j];

            }
            else
            {
                c_src_data[j] = c_edge_img_data[j] > c_src_data[j] ? c_edge_img_data[j] : c_src_data[j];
                k_src_data[j] = k_edge_img_data[j] > k_src_data[j] ? k_edge_img_data[j] : k_src_data[j];
                m_src_data[j] = m_edge_img_data[j] > m_src_data[j] ? m_edge_img_data[j] : m_src_data[j];
                y_src_data[j] = y_edge_img_data[j] > y_src_data[j] ? y_edge_img_data[j] : y_src_data[j];
            }
        }
    }
    imwrite("./cmyk/c_src_result.jpg", c_src_img);
    imwrite("./cmyk/k_src_result.jpg", k_src_img);
    imwrite("./cmyk/m_src_result.jpg", m_src_img);
    imwrite("./cmyk/y_src_result.jpg", y_src_img);
}
void add_img(Mat &src, Mat &edge, Mat &out)
{

    //参数准备
    out = src.clone();
    int rowNumber = src.rows;  //行数
    int colNumber = src.cols;  //列数 x 通道数=每一行元素的个数

    for (int i = 0; i < rowNumber; i++)
    {
        uchar* src_data = src.ptr<uchar>(i);
        uchar* edge_data = edge.ptr<uchar>(i);
        uchar* output_data = out.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            
            // if ((src_data[j * 3 + 0] > 250 && src_data[j * 3 + 1] > 250 && src_data[j * 3 + 2] > 250) &&
            //    (edge_data[j * 3 + 0] < 250 || edge_data[j * 3 + 1]  < 250 || edge_data[j * 3 + 2] < 250))
            if (src_data[j]>250 && edge_data[j] < 250)
            {
                output_data[j] = edge_data[j];
                /*output_data[j * 3 + 0] = edge_data[j * 3 + 0];
                output_data[j * 3 + 1] = edge_data[j * 3 + 1];
                output_data[j * 3 + 2] = edge_data[j * 3 + 2];*/
            }
            //else if((edge_data[j * 3 + 0] > 250 && edge_data[j * 3 + 1] > 250 && edge_data[j * 3 + 2] > 250) &&
            //    (src_data[j * 3 + 0] < 250 || src_data[j * 3 + 1] < 250 || src_data[j * 3 + 2] < 250))
            else if (edge_data[j] > 250 && src_data[j] < 250)
            {
                output_data[j] = src_data[j];
              /*  output_data[j * 3 + 0] = src_data[j * 3 + 0];
                output_data[j * 3 + 1] = src_data[j * 3 + 1];
                output_data[j * 3 + 2] = src_data[j * 3 + 2];*/
            }
            else
            {
                output_data[j] = edge_data[j] < src_data[j] ? edge_data[j]: src_data[j];

            /*    output_data[j * 3 + 0] = (edge_data[j * 3 + 0] < src_data[j * 3 + 0]) ? edge_data[j * 3 + 0] : src_data[j * 3 + 0];
                output_data[j * 3 + 1] = (edge_data[j * 3 + 1] < src_data[j * 3 + 1]) ? edge_data[j * 3 + 1] : src_data[j * 3 + 1];
                output_data[j * 3 + 2] = (edge_data[j * 3 + 2] < src_data[j * 3 + 2]) ? edge_data[j * 3 + 2] : src_data[j * 3 + 2];*/
            }
        }
    }
}
void find_contours(Mat &in, Mat &out, int mode)
{
        //外轮廓
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        if (mode == 1)
        {
            findContours(in, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);
        }
        else
            findContours(in, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        Mat contoursImage(in.rows, in.cols, CV_8U, Scalar(0));
        //for (int i = 0; i<contours.size(); i++){
        //    if (hierarchy[i][3] != -1) drawContours(contoursImage, contours, i, Scalar(255), 3);
        //}
        for (int i = 0; i< contours.size(); i++)
        {
            Scalar color = Scalar(255,255,255);
            drawContours(contoursImage, contours, i, color, 2, 8, hierarchy, 0, Point());
        }
        contoursImage.copyTo(out);
}
void to_black_img(Mat &in, Mat &out)
{
    //参数准备
    int rowNumber = in.rows;  //行数
    int colNumber = in.cols;  //列数 x 通道数=每一行元素的个数
    //in.copyTo(out);
    out.create(rowNumber, colNumber, CV_8UC1);
    for (int i = 0; i < rowNumber; i++)
    {
        uchar* in_img_data = in.ptr<uchar>(i);
        uchar* out_img_data = out.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            uchar min = std::min( std::min(in_img_data[j * 3 + 0], in_img_data[j * 3 + 1]), in_img_data[j * 3 + 2]);
            if (min < 250)
                out_img_data[j] = min / 2;
            else
                out_img_data[j] = 255;
        }
    }

}
void creat_real_color_canny_edge(Mat &srcImage, Mat &canny_img, Mat &dst_img)
{
    //参数准备
    int rowNumber = srcImage.rows;  //行数
    int colNumber = srcImage.cols;  //列数 x 通道数=每一行元素的个数
    //srcImage.copyTo(dst_img);
    // dst_img.zero
    // dst_img = cv::Mat::zeros(Size(srcImage.cols, srcImage.rows), CV_32F);
    dst_img = srcImage.clone();
    for (int i = 0; i < rowNumber; i++)
    {
        uchar* dst_img_data = dst_img.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            dst_img_data[j * 3 + 0] = 0;
            dst_img_data[j * 3 + 1] = 0;
            dst_img_data[j * 3 + 2] = 0;
        }
    }

    for (int i = 0; i < rowNumber; i++)
    {
        uchar* src_data = srcImage.ptr<uchar>(i);
        uchar* canny_img_data = canny_img.ptr<uchar>(i);
        uchar* dst_img_data = dst_img.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            if (canny_img_data[j] > 250)
            {
                if (src_data[j * 3 + 0] > 230 && src_data[j * 3 + 1] > 230 && src_data[j * 3 + 2] > 230)
                {
                    continue;
                }
                dst_img_data[j * 3 + 0] = src_data[j * 3 + 0];
                dst_img_data[j * 3 + 1] = src_data[j * 3 + 1];
                dst_img_data[j * 3 + 2] = src_data[j * 3 + 2];
            }
        }
    }
}
void color_mix_bak(Mat& srcImage, Mat& dilateImage, Mat& outputImage, int add_width)
{
    //参数准备
    outputImage = srcImage.clone();
    int rowNumber = srcImage.rows;  //行数
    int colNumber = srcImage.cols;  //列数 x 通道数=每一行元素的个数

    for (int i = 0; i < rowNumber; i++)
    {
        uchar* src_data = srcImage.ptr<uchar>(i);
        uchar* dilate_data = dilateImage.ptr<uchar>(i);
        uchar* output_data = outputImage.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            if (dilate_data[j] > 250)
            {
                cv::Point for_point(i, j);
                cv::Point finded_point(0, 0);
                find_insert_point(srcImage, for_point, finded_point, add_width);
                //cout << for_point.x << "," << for_point.y << "|" << finded_point.x << "," << finded_point.y << endl;
                //cout << "---------------" << endl;
                if (finded_point.x == 0 && finded_point.y == 0)
                {
                    output_data[j * 3 + 0] = src_data[j * 3 + 0];
                    output_data[j * 3 + 1] = src_data[j * 3 + 1];
                    output_data[j * 3 + 2] = src_data[j * 3 + 2];
                }
                else
                {
                    output_data[j * 3 + 0] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[0];
                    output_data[j * 3 + 1] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[1];
                    output_data[j * 3 + 2] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[2];
                }
            }
            else
            {
                output_data[j * 3 + 0] = src_data[j * 3 + 0];
                output_data[j * 3 + 1] = src_data[j * 3 + 1];
                output_data[j * 3 + 2] = src_data[j * 3 + 2];
            }
        }
    }

}
void color_mix(Mat& srcImage, Mat& dilateImage, Mat& outputImage, int add_width)
{
    //参数准备
    outputImage = srcImage.clone();
    int rowNumber = srcImage.rows;  //行数
    int colNumber = srcImage.cols;  //列数 x 通道数=每一行元素的个数

    for (int i = 0; i < rowNumber; i++)
    {
        uchar* src_data = srcImage.ptr<uchar>(i);
        uchar* dilate_data = dilateImage.ptr<uchar>(i);
        uchar* output_data = outputImage.ptr<uchar>(i);
        for (int j = 0; j < colNumber; j++)
        {
            if (dilate_data[j] > 250)
            {
                cv::Point for_point(i, j);
                cv::Point finded_point(0, 0);
                find_insert_point(srcImage, for_point, finded_point, add_width);
                //cout << for_point.x << "," << for_point.y << "|" << finded_point.x << "," << finded_point.y << endl;
                //cout << "---------------" << endl;
                if (finded_point.x == 0 && finded_point.y == 0)
                {
                    //output_data[j] = src_data[j];
                    output_data[j * 3 + 0] = src_data[j * 3 + 0];
                    output_data[j * 3 + 1] = src_data[j * 3 + 1];
                    output_data[j * 3 + 2] = src_data[j * 3 + 2];
                }
                else
                {
                   // output_data[j] = srcImage.at<uchar>(finded_point.x, finded_point.y);
                    output_data[j * 3 + 0] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[0];
                    output_data[j * 3 + 1] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[1];
                    output_data[j * 3 + 2] = srcImage.at<Vec3b>(finded_point.x, finded_point.y)[2];
                }
            }
            else
            {
               // output_data[j] = 255;
                output_data[j * 3 + 0] = 255;
                output_data[j * 3 + 1] = 255;
                output_data[j * 3 + 2] = 255;
            }
        }
    }

}

void color_mix_cmyk(Mat& srcImage, Mat& dilateImage, int add_width)
{
    //参数准备

    Mat c_src_img = imread("./cmyk/c_src.jpg", 0);
    Mat k_src_img = imread("./cmyk/k_src.jpg", 0);
    Mat m_src_img = imread("./cmyk/m_src.jpg", 0);
    Mat y_src_img = imread("./cmyk/y_src.jpg", 0);

    Mat c_outputImage = dilateImage.clone();
    Mat k_outputImage = dilateImage.clone();
    Mat m_outputImage = dilateImage.clone();
    Mat y_outputImage = dilateImage.clone();

    int rowNumber = srcImage.rows;  //行数
    int colNumber = srcImage.cols;  //列数 x 通道数=每一行元素的个数

    for (int i = 0; i < rowNumber; i++)
    {
        uchar* src_data = srcImage.ptr<uchar>(i);
        uchar* dilate_data = dilateImage.ptr<uchar>(i);

        uchar* c_outputImage_data = c_outputImage.ptr<uchar>(i);
        uchar* k_outputImage_data = k_outputImage.ptr<uchar>(i);
        uchar* m_outputImage_data = m_outputImage.ptr<uchar>(i);
        uchar* y_outputImage_data = y_outputImage.ptr<uchar>(i);
        
        uchar* c_src_img_data = c_src_img.ptr<uchar>(i);
        uchar* k_src_img_data = k_src_img.ptr<uchar>(i);
        uchar* m_src_img_data = m_src_img.ptr<uchar>(i);
        uchar* y_src_img_data = y_src_img.ptr<uchar>(i);

        for (int j = 0; j < colNumber; j++)
        {

            if (dilate_data[j] < 250)
            {
                c_outputImage_data[j] = 0;
                k_outputImage_data[j] = 0;
                m_outputImage_data[j] = 0;
                y_outputImage_data[j] = 0;
            }
            else
            {
                cv::Point for_point(i, j);
                cv::Point finded_point(0, 0);
                find_insert_point(srcImage, for_point, finded_point, add_width);
                //cout << for_point.x << "," << for_point.y << "|" << finded_point.x << "," << finded_point.y << endl;
                //cout << "---------------" << endl;
                if (finded_point.x == 0 && finded_point.y == 0)
                {
                    //output_data[j] = src_data[j];
                    c_outputImage_data[j] = c_src_img_data[j];
                    k_outputImage_data[j] = k_src_img_data[j];
                    m_outputImage_data[j] = m_src_img_data[j];
                    y_outputImage_data[j] = y_src_img_data[j];
                }
                else
                {
                    c_outputImage_data[j] = c_src_img.at<uchar>(finded_point.x, finded_point.y);
                    k_outputImage_data[j] = k_src_img.at<uchar>(finded_point.x, finded_point.y);
                    m_outputImage_data[j] = m_src_img.at<uchar>(finded_point.x, finded_point.y);
                    y_outputImage_data[j] = y_src_img.at<uchar>(finded_point.x, finded_point.y);
                    
                }
            }
        }
    }
  
    //std::cout << "cmyk  edge gen ok" << std::endl;
    ////腐蚀
    //for (int i = int(add_width / 2); i < rowNumber - int(add_width / 2); i++)
    //{
    //    uchar* c_outputImage_data = c_outputImage.ptr<uchar>(i);
    //    uchar* k_outputImage_data = k_outputImage.ptr<uchar>(i);
    //    uchar* m_outputImage_data = m_outputImage.ptr<uchar>(i);
    //    uchar* y_outputImage_data = y_outputImage.ptr<uchar>(i);
    //    
    //    for (int j = int(add_width / 2); j < colNumber - int(add_width / 2); j++)
    //    {
    //        int min_point_x = i;
    //        int min_point_y = j;
    //        int min = 100000;
    //        int x = i;
    //        int y = j;
    //        for (x = i - int(add_width / 2); x < i + int(add_width / 2); x++)
    //        {
    //            for (y = j - int(add_width / 2); y < j + int(add_width / 2); y++)
    //            {
    //                int pix_sum = c_outputImage_data[j] + k_outputImage_data[j] + m_outputImage_data[j] + y_outputImage_data[j];
    //                if (pix_sum < min)
    //                {
    //                    min = pix_sum;
    //                    min_point_x = x;
    //                    min_point_y = y;
    //                }
    //            }
    //        }
    //        uchar* c_outputImage_data_x = c_outputImage.ptr<uchar>(x);
    //        uchar* k_outputImage_data_x = k_outputImage.ptr<uchar>(x);
    //        uchar* m_outputImage_data_x = m_outputImage.ptr<uchar>(x);
    //        uchar* y_outputImage_data_x = y_outputImage.ptr<uchar>(x);

    //        c_outputImage_data[j] = c_outputImage_data_x[min_point_y];
    //        k_outputImage_data[j] = k_outputImage_data_x[min_point_y];
    //        m_outputImage_data[j] = m_outputImage_data_x[min_point_y];
    //        y_outputImage_data[j] = y_outputImage_data_x[min_point_y];
    //    }
    //}
    cv::imwrite("./cmyk/c_edge.jpg", c_outputImage);
    cv::imwrite("./cmyk/k_edge.jpg", k_outputImage);
    cv::imwrite("./cmyk/m_edge.jpg", m_outputImage);
    cv::imwrite("./cmyk/y_edge.jpg", y_outputImage);

}
void find_insert_point(Mat &src_img, cv::Point &for_point, cv::Point &out_point, int add_width)
{
    int offset = 10;
    int threshhold = 240;
    int offset2 = 10;
    for (int i = 0; i < add_width; i++)
    {
        if (for_point.x + 2 * i  < src_img.rows)
        {
            uchar* src_img_data1 = src_img.ptr<uchar>(for_point.x + i);
            {
                if ((src_img_data1[for_point.y * 3 + 0] < threshhold || src_img_data1[for_point.y * 3 + 2] < threshhold || src_img_data1[for_point.y * 3 + 1] < threshhold))
                //if (src_img_data1[for_point.y] < threshhold)
                {
                    out_point.x = for_point.x + 2 * i;
                    int real_idx = for_point.y - offset2;
                    int add_data = src_img_data1[real_idx * 3 + 0] + src_img_data1[real_idx * 3 + 1] + src_img_data1[real_idx * 3 + 2];
                    //int add_data = src_img_data1[real_idx ];
                    int read_idx2 = for_point.y + offset2;
                    int add_data2 = src_img_data1[read_idx2 * 3 + 0] + src_img_data1[read_idx2 * 3 + 1] + src_img_data1[read_idx2 * 3 + 2];
                   // int add_data2 = src_img_data1[read_idx2];
                    if (add_data > add_data2 && for_point.y + offset2 < src_img.cols)
                    {
                        out_point.y = for_point.y + offset2;
                    }
                    else if (for_point.y - offset2 >0)
                    {
                        out_point.y = for_point.y - offset2;
                    }
                    else
                        out_point.y = for_point.y;
                    return;
                }
            }
        }
        uchar* src_img_data2 = src_img.ptr<uchar>(for_point.x);
        {
            if (for_point.y - (i * 2) > 0)
            {
                if (src_img_data2[(for_point.y - i) * 3 + 0] < threshhold || src_img_data2[(for_point.y - i) * 3 + 2] < threshhold || src_img_data2[(for_point.y - i) * 3 + 1] < threshhold)
                //if(src_img_data2[(for_point.y - i)] < threshhold)
                {
                    int add_data = 0, add_data2 = 0;
                    if (for_point.x + offset2 < src_img.rows && for_point.x - offset2 > 0)
                    {
                        uchar* src_img_data_real1 = src_img.ptr<uchar>(for_point.x + offset2);
                        add_data = src_img_data_real1[(for_point.y - i) * 3 + 0] + src_img_data_real1[(for_point.y - i) * 3 + 2] + src_img_data_real1[(for_point.y - i) * 3 + 1];
                        //add_data = src_img_data_real1[(for_point.y - i)];
                        uchar* src_img_data_real2 = src_img.ptr<uchar>(for_point.x - offset2);
                        //add_data2 = src_img_data_real2[(for_point.y - i)];
                        add_data2 = src_img_data_real2[(for_point.y - i) * 3 + 0] + src_img_data_real2[(for_point.y - i) * 3 + 2] + src_img_data_real2[(for_point.y - i) * 3 + 1];
                    }
                    if (add_data > add_data2)
                    {
                        out_point.x = for_point.x - offset2;
                    }
                    else if (add_data2 > add_data)
                    {
                        out_point.x = for_point.x + offset2;
                    }
                    else
                        out_point.x = for_point.x;
                    out_point.y = for_point.y - (i * 2);
                    return;
                }
            }
            if (src_img_data2[(for_point.y + i) * 3 + 0] < threshhold || src_img_data2[(for_point.y + i) * 3 + 2] < threshhold || src_img_data2[(for_point.y + i) * 3 + 1] < threshhold)
            //if (src_img_data2[(for_point.y + i)] < threshhold)
            {
                if (for_point.y + (i * 2) < src_img.cols)
                {
                    int add_data = 0, add_data2 = 0;
                    if (for_point.x + offset2 < src_img.rows && for_point.x - offset2 > 0)
                    {
                        uchar* src_img_data_real1 = src_img.ptr<uchar>(for_point.x + offset2);
                        add_data = src_img_data_real1[(for_point.y + i) * 3 + 0] + src_img_data_real1[(for_point.y + i) * 3 + 2] + src_img_data_real1[(for_point.y + i) * 3 + 1];
                        //add_data = src_img_data_real1[(for_point.y + i)];
                        uchar* src_img_data_real2 = src_img.ptr<uchar>(for_point.x - offset2);
                        //add_data2 = src_img_data_real2[(for_point.y + i)];
                        add_data2 = src_img_data_real2[(for_point.y + i) * 3 + 0] + src_img_data_real2[(for_point.y + i) * 3 + 2] + src_img_data_real2[(for_point.y + i) * 3 + 1];
                    }
                    if (add_data > add_data2)
                    {
                        out_point.x = for_point.x - offset2;
                    }
                    else if (add_data2 > add_data)
                    {
                        out_point.x = for_point.x + offset2;
                    }
                    else
                        out_point.x = for_point.x;
                    out_point.y = for_point.y + (i * 2);
                    return;
                }
            }

        }
        if (for_point.x - (i + offset) > 0)
        {
            uchar* src_img_data3 = src_img.ptr<uchar>(for_point.x - i);
            if (src_img_data3[for_point.y * 3 + 0] < threshhold || src_img_data3[for_point.y * 3 + 2] < threshhold || src_img_data3[for_point.y * 3 + 1] < threshhold)
            //if (src_img_data3[for_point.y] < threshhold)
            {
                out_point.x = for_point.x - (i + offset);
                int real_idx = for_point.y - offset2;
                int add_data = src_img_data3[real_idx * 3 + 0] + src_img_data3[real_idx * 3 + 1] + src_img_data3[real_idx * 3 + 2];
                //int add_data = src_img_data3[real_idx];
                int read_idx2 = for_point.y + offset2;
                //int add_data2 = src_img_data3[read_idx2];
                int add_data2 = src_img_data3[read_idx2 * 3 + 0] + src_img_data3[read_idx2 * 3 + 1] + src_img_data3[read_idx2 * 3 + 2];
                if (add_data > add_data2 && for_point.y + offset2 < src_img.cols)
                {
                    out_point.y = for_point.y + offset2;
                }
                else if (for_point.y - offset2 >0)
                {
                    out_point.y = for_point.y - offset2;
                }
                else
                    out_point.y = for_point.y;
                return;
            }
        }
    }
}
void colorReduce(Mat& inputImage, Mat& outputImage, int add_width)
{

    cv::Rect roi_rect = cv::Rect(add_width, add_width, inputImage.cols, inputImage.rows);
    inputImage.copyTo(outputImage(roi_rect));

    //参数准备
    Mat tempImage = inputImage.clone();
    int rowNumber = tempImage.rows;  //行数
    int colNumber = tempImage.cols;  //列数 x 通道数=每一行元素的个数
    //上
    for (int i = 0; i < add_width; i++)
    {
        uchar* data_start = outputImage.ptr<uchar>(add_width);
        uchar* data = outputImage.ptr<uchar>(i);
        for (int j = 0; j < colNumber + 2 * add_width; j++)
        {
            if (j < add_width)
            {
                data[j * 3] = data_start[add_width * 3 + 0];
                data[j * 3 + 1] = data_start[add_width * 3 + 1];
                data[j * 3 + 2] = data_start[add_width * 3 + 2];
            }
            else if (j > colNumber + add_width - 1)
            {
                data[j * 3] = data_start[(colNumber + add_width - 1) * 3 + 0];
                data[j * 3 + 1] = data_start[(colNumber + add_width - 1) * 3 + 1];
                data[j * 3 + 2] = data_start[(colNumber + add_width - 1) * 3 + 2];
            }
            else
            {
                data[j * 3] = data_start[j * 3];
                data[j * 3 + 1] = data_start[j * 3 + 1];
                data[j * 3 + 2] = data_start[j * 3 + 2];
            }
        }
    }
    //下
    for (int i = add_width + rowNumber; i < 2 * add_width + rowNumber; i++)
    {
        uchar* data_start = outputImage.ptr<uchar>(add_width + rowNumber - 1);
        uchar* data = outputImage.ptr<uchar>(i);
        for (int j = 0; j < colNumber + 2 * add_width; j++)
        {
            if (j < add_width)
            {
                data[j * 3] = data_start[add_width * 3 + 0];
                data[j * 3 + 1] = data_start[add_width * 3 + 1];
                data[j * 3 + 2] = data_start[add_width * 3 + 2];
            }
            else if (j > colNumber + add_width - 1)
            {
                data[j * 3] = data_start[(colNumber + add_width - 1) * 3 + 0];
                data[j * 3 + 1] = data_start[(colNumber + add_width - 1) * 3 + 1];
                data[j * 3 + 2] = data_start[(colNumber + add_width - 1) * 3 + 2];
            }
            else
            {
                data[j * 3] = data_start[j * 3];
                data[j * 3 + 1] = data_start[j * 3 + 1];
                data[j * 3 + 2] = data_start[j * 3 + 2];
            }
        }
    }
    //左
    for (int i = add_width; i < add_width + rowNumber; i++)
    {
        uchar* data = outputImage.ptr<uchar>(i);
        for (int j = 0; j < add_width; j++)
        {
            data[j * 3] = data[add_width * 3 + 0];
            data[j * 3 + 1] = data[add_width * 3 + 1];
            data[j * 3 + 2] = data[add_width * 3 + 2];
        }
    }

    //右
    for (int i = add_width; i < add_width + rowNumber; i++)
    {
        uchar* data = outputImage.ptr<uchar>(i);
        int read_pix = colNumber + add_width - 1;
        for (int j = colNumber + add_width; j < colNumber + 2 * add_width; j++)
        {
            data[j * 3] = data[read_pix * 3 + 0];
            data[j * 3 + 1] = data[read_pix * 3 + 1];
            data[j * 3 + 2] = data[read_pix * 3 + 2];
        }
    }
}


//-----------------------------------【ShowHelpText( )函数】----------------------------------
//          描述：输出一些帮助信息
//----------------------------------------------------------------------------------------------
void ShowHelpText()
{
    //输出欢迎信息和OpenCV版本
    printf("\n\n   当前使用的OpenCV版本为：" CV_VERSION);
    printf("\n\n 需要输入两个参数 \n");
    printf(" param 1：表示要扩充的宽度（像素个数） \n");
    printf(" param 2：输入图像路径 \n");
}

