/******
 * 包含CANNY算子实现 和 霍夫圆变换（霍夫梯度法）
 ******/

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

enum EdgeDetectorType{
    SOBEL=1,
    ROBERTS,
    PREWITT,
    LOG,
    CANNY
};

/***************函数声明***************/
Mat EdgeDetector(Mat input, EdgeDetectorType edge_detector_type);
Mat HoughCircles(Mat input, Mat edge_circ);
Mat Convolution2D(Mat &input, Mat kernel);
Mat GaussianKernel(double sigma, int T_size);
Mat BinaryImage(Mat &input, double threshold);

Mat raw;

int main(int argc, char *argv[])
{
    Mat raw_circle = imread("./src/HW3/data/circle.png");
    Mat lena = imread("./src/HW3/data/lena.bmp");
    
   /***************读取图像***************/
    if (!raw_circle.data || !lena.data)
    {
        cout << "error while loading images" << endl;
        return 0;
    }
    imshow("raw_circle", raw_circle);
    Mat gray_circle, lena_gray;
    cvtColor(lena, lena_gray, COLOR_BGR2GRAY);
    cvtColor(raw_circle, gray_circle, COLOR_BGR2GRAY);

    /****************调用边缘检测函数****************/
    Mat lena_edge_sobel = EdgeDetector(lena_gray,SOBEL);
    Mat lena_edge_roberts = EdgeDetector(lena_gray,ROBERTS);
    Mat lena_edge_prewitt = EdgeDetector(lena_gray,PREWITT);
    Mat lena_edge_log= EdgeDetector(lena_gray,LOG);
    Mat lena_edge_canny= EdgeDetector(lena_gray,CANNY);
    Mat lena_edge_canny_2 = Mat::zeros(lena.size(),CV_8U);
    Canny(lena_gray, lena_edge_canny_2, 50, 80);
    imshow("edge_detect_result(sobel)", lena_edge_sobel);
    imshow("edge_detect_result(roberts)", lena_edge_roberts);
    imshow("edge_detect_result(prewitt)", lena_edge_prewitt);
    imshow("edge_detect_result(log)", lena_edge_log);
    imshow("edge_detect_result(canny)", lena_edge_canny);
    // 调用内置函数，输出的边缘检测图
    imshow("opencv canny", lena_edge_canny_2);

    /***************调用霍夫圆变换***************/
    Mat edge_circle= EdgeDetector(gray_circle,CANNY);
    Mat circ_detect = HoughCircles(gray_circle, edge_circle);
    imshow("hough circle transform", circ_detect);
    waitKey(0);
    return 0;
}
// 利用高斯函数生成模板     
Mat GaussianKernel(double sigma, int T_size){ // 模板大小 T_size = 2N+1                      
    Mat Template = Mat::zeros(T_size, T_size, CV_64F); // 初始化模板矩阵
    int center = round(T_size / 2);                    // 模板中心位置, N
    double sum = 0.0;
    // double factor = 0.5/(CV_PI*sigma*sigma);

    for (int i = 0; i < T_size; i++)
    {
        int a = (i-center)*(i-center);
        for (int j = 0; j < T_size; j++)
        {
            int b = (j-center) * (j-center);
            // Template.at<double>(i, j) = factor * exp(-1.0*(a+b)/(2*sigma*sigma));   
            Template.at<double>(i, j) = exp(-1.0*(a+b)/(2*sigma*sigma));        
            sum += Template.at<double>(i, j); //用于归一化模板元素
        }
    }

    for (int i = 0; i < T_size; i++)
    {
        for (int j = 0; j < T_size; j++)
        {
            /*** 模板归一化代码 ***/
            double temp = Template.at<double>(i, j);
            Template.at<double>(i, j) = temp/sum;
        }
    }
    return Template;
}
// 卷积
Mat Convolution2D(Mat &input, Mat kernel)
{
     int T_size = kernel.cols;
     int center = round(T_size / 2);      // 模板中心位置, N
   
    input.convertTo(input, CV_64F);
    Mat output = Mat::zeros(input.size(), CV_64F);
    int startnum = -1*center;
    int endnum = T_size - center;
   
    for (int m = 0; m < input.rows; ++m){
        for (int n = 0; n < input.cols; ++n){
        double result = 0.0;
            for(int i = startnum; i < endnum; ++i){
                for(int j = startnum; j < endnum; ++j){
                    if(m+i<0 || n+j<0 || m+i>=input.rows || n+j>=input.cols) continue;
                    else result += kernel.at<double>(center+i, center+j) *input.at<double>(m+i, n+j);
                }
            }
            output.at<double>(m,n) = result;
        }
    }

    return output;
}
// 二值化
Mat BinaryImage(Mat &input, double threshold){
    Mat output = Mat::zeros(input.size(), CV_8U);
    Mat Src = input.clone();
    Src.convertTo(Src, CV_64F);
    for(int i = 0; i<input.rows; ++i){
        for(int j = 0; j<input.cols; ++j){
            if(Src.at<double>(i,j) > threshold) output.at<uint8_t>(i,j) = 255;
        }
    }
    return output;
}
/***************下面实现EdgeDetector()函数***************/
Mat EdgeDetector(Mat input, EdgeDetectorType edge_detector_type){
    Mat gray_image = input.clone();
    double sigma = 2.5;
    Mat result;
    Mat gaussian_kernel = GaussianKernel(sigma, 9);
    double threshold = 35;
    switch(edge_detector_type){
    case SOBEL:{
        Mat GaussianResult = Convolution2D(gray_image, gaussian_kernel);
        Mat sobel_kernel_y= (cv::Mat_<double>(3,3) <<-1,-2,-1,0,0,0,1,2,1);
        Mat sobel_kernel_x = (cv::Mat_<double>(3,3) <<-1,0,1,-2,0,2,-1,0,1);
        Mat resultx = Convolution2D(GaussianResult,sobel_kernel_x);
        Mat resulty = Convolution2D(GaussianResult,sobel_kernel_y);
        result = Mat::zeros(input.size(),CV_64F);
        for(int m=0;m<input.rows;++m)
            for(int n=0;n<input.cols;++n){
                double temp = abs(resultx.at<double>(m,n)) + abs(resulty.at<double>(m,n)); 
                if(temp>255) result.at<double>(m,n) = 255;
                else result.at<double>(m,n) = temp;
            }
        return BinaryImage(result, threshold);
        break;
    } 
    case ROBERTS:{
        Mat GaussianResult = Convolution2D(gray_image, gaussian_kernel);
        Mat roberts_kernel_x = (cv::Mat_<double>(2,2) <<-1,0,0,1);
        Mat roberts_kernel_y = (cv::Mat_<double>(2,2) <<0,-1,1,0);
        Mat result_x= Convolution2D(GaussianResult,roberts_kernel_x);
        Mat result_y= Convolution2D(GaussianResult,roberts_kernel_y);
        result = Mat::zeros(input.size(),CV_64F);
        for(int m=0;m<input.rows;++m)
            for(int n=0;n<input.cols;++n){
                double temp = abs(result_x.at<double>(m,n)) + abs(result_y.at<double>(m,n)); 
                if(temp>255) result.at<double>(m,n) = 255;
                else result.at<double>(m,n) = temp;
            }
        double threshold = 15;
        return BinaryImage(result, threshold);
        break; 
    }
    case PREWITT:{
        Mat GaussianResult = Convolution2D(gray_image, gaussian_kernel);
        Mat prewitt_kernel_y = (cv::Mat_<double>(3,3) <<-1,-1,-1,0,0,0,1,1,1);
        Mat prewitt_kernel_x = (cv::Mat_<double>(3,3) <<-1,0,1,-1,0,1,-1,0,1);
        Mat resultx = Convolution2D(GaussianResult,prewitt_kernel_x);
        Mat resulty = Convolution2D(GaussianResult,prewitt_kernel_y);
        result = Mat::zeros(input.size(),CV_64F);
        for(int m=0;m<input.rows;++m)
            for(int n=0;n<input.cols;++n){
                double temp = abs(resultx.at<double>(m,n)) + abs(resulty.at<double>(m,n)); 
                if(temp>255) result.at<double>(m,n) = 255;
                else result.at<double>(m,n) = temp;
            }
        return BinaryImage(result, threshold);
        break;
    }
    case LOG:{
        Mat log_kernel = (cv::Mat_<double>(5,5) <<0, 0, -1, 0, 0,
                                                 0, -1, -2, -1, 0,
                                                -1, -2, 16, -2, -1,
                                                 0, -1, -2, -1, 0,
                                                  0, 0, -1, 0, 0);
        result= Convolution2D(gray_image,log_kernel);
        return BinaryImage(result, threshold);
        break;
    }
    case CANNY:{
        // Mat GaussianResult = Convolution2D(gray_image, gaussian_kernel); // 平滑
        Mat GaussianResult = gray_image;
        // 利用sobel算子计算梯度
        Mat sobel_kernel_y = (cv::Mat_<double>(3,3) <<-1,-2,-1,0,0,0,1,2,1);
        Mat g_y = Convolution2D(GaussianResult,sobel_kernel_y);
        Mat sobel_kernel_x = (cv::Mat_<double>(3,3) <<-1,0,1,-2,0,2,-1,0,1);
        Mat g_x = Convolution2D(GaussianResult,sobel_kernel_x);
        // 非极大值抑制
        Mat grad_mag = Mat::zeros(input.size(),CV_64F);
        for(int m=0;m<input.rows;++m)
            for(int n=0;n<input.cols;++n)
                grad_mag.at<double>(m,n) = sqrt(g_x.at<double>(m,n) * g_x.at<double>(m,n) + g_y.at<double>(m,n) * g_y.at<double>(m,n));  // 梯度幅值

        Mat g_N = grad_mag.clone();
        for(int m=0;m<input.rows;++m){// corresponds to y
        for(int n=0;n<input.cols;++n){// corresponds to x
            double grad_x = g_x.at<double>(m,n);
            double grad_y = g_y.at<double>(m,n);
            double y_forward = grad_y/grad_x;
            double value1, value2, max_value;
            if(y_forward>=-1*tan(CV_PI/8) && y_forward <= tan(CV_PI/8)){// horizontal
                if(n-1<0) value1=0;
                else value1 = grad_mag.at<double>(m,n-1);
                if(n+1>=input.cols) value2=0;
                else value2 = grad_mag.at<double>(m,n+1);
            }
            else if(y_forward>=tan(CV_PI/8) && y_forward <= tan(CV_PI/8*3)){//45 deg
                if(n-1<0 || m-1 <0) value1=0;
                else value1 = grad_mag.at<double>(m-1,n-1);
                if(n+1>=input.cols || m+1>=input.rows) value2=0;
                else value2 = grad_mag.at<double>(m+1,n+1);
            }
            else if(y_forward<=-1*tan(CV_PI/8) && y_forward >= -1*tan(CV_PI/8*3)){//-45 deg
                if(n-1<0 || m+1 >=input.rows) value1=0;
                else value1 = grad_mag.at<double>(m+1,n-1);
                if(n+1>=input.cols || m-1<0) value2=0;
                else value2 = grad_mag.at<double>(m-1,n+1);
            }
            else{//vertical
                if(m+1 >=input.rows) value1=0;
                else value1 = grad_mag.at<double>(m+1,n);
                if(m-1<0) value2=0;
                else value2 = grad_mag.at<double>(m-1,n);
            }
            max_value = max(value1,value2);
            if(grad_mag.at<double>(m,n) < max_value) g_N.at<double>(m,n) =0;
        }
    }
        
        Mat g_temp,g_temp2;
        g_N.convertTo(g_temp, CV_8U);
        grad_mag.convertTo(g_temp2,CV_8U);
        imshow("after NMS",g_temp);
        imshow("before NMS",g_temp2);
        // 双阈值分割
        Mat g_NH = Mat::zeros(input.size(),CV_8UC1);
        Mat g_NL = Mat::zeros(input.size(),CV_8UC1);
        for(int m=0;m<input.rows;++m){
        for(int n=0;n<input.cols;++n){
            if(g_N.at<double>(m,n) > 80) g_NH.at<uint8_t>(m,n) = 255;
            if(g_N.at<double>(m,n) > 50) g_NL.at<uint8_t>(m,n) = 255;
        }
        }
        Mat checked = Mat::zeros(input.size(),CV_8UC1);
        // 边缘连接
        for(int m=0;m<input.rows;++m){
        for(int n=0;n<input.cols;++n){
            if(g_NH.at<uint8_t>(m,n) == 0) continue;
            for(int i = -1; i <= 1; ++i){
                for(int j = -1; j <= 1; ++j){
                    if(m+i<0 || n+j<0 || m+i>=input.rows || n+j>=input.cols) continue;
                    if (g_NL.at<uint8_t>(m+i,n+j) == 255) checked.at<uint8_t>(m+i,n+j) = 255;
                }
            }
            }
        }
        imshow("g_NH",g_NH);
        imshow("g_NL",g_NL);
        for(int m=0;m<input.rows;++m)
            for(int n=0;n<input.cols;++n)
                if(checked.at<uint8_t>(m,n) == 255) g_NH.at<uint8_t>(m,n) = 255;
        return g_NH;
        break;
    }
    }
}

/***************下面实现HoughCircles()函数***************/
Mat HoughCircles(Mat input, Mat edge_circ){
    double sigma = 2.5;
    Mat gray_image = input.clone();
    Mat gaussian_kernel = GaussianKernel(sigma, 9);
    Mat GaussianResult = Convolution2D(gray_image, gaussian_kernel);
    // Compute gradient
    Mat sobel_kernel_x = (cv::Mat_<double>(3,3) <<-1,-2,-1,0,0,0,1,2,1);
    Mat g_y = Convolution2D(GaussianResult,sobel_kernel_x);
    Mat sobel_kernel_y = (cv::Mat_<double>(3,3) <<-1,0,1,-2,0,2,-1,0,1);
    Mat g_x = Convolution2D(GaussianResult,sobel_kernel_y);
    
    int accumulator[input.rows][input.cols] = {0};
    for(int m=0;m<input.rows;++m){
        for(int n=0;n<input.cols;++n){
            if(edge_circ.at<uint8_t>(m,n)==0) continue;
            double grad_x = g_x.at<double>(m,n);
            double grad_y = g_y.at<double>(m,n);
            double x_forward = grad_x/abs(grad_x);
            double y_forward = grad_y/abs(grad_x);
            for(int k=0;;++k){
                if(n+k*x_forward<0 || m+k*y_forward>=input.rows || n+k*x_forward>=input.cols || m+k*y_forward<0) break;
                int n_cur = n+k*x_forward, m_cur = m+k*y_forward;
                accumulator[m_cur][n_cur]++;
            }
        }
    }
    const int center_num =5;
    int circ_x[center_num], circ_y[center_num];
    for(int i=0;i<center_num;++i){
        int max_indicator=0, m_indicator=0, n_indicator=0;
        for(int m=0;m<input.rows;++m){
            for(int n=0;n<input.cols;++n){
                if(max_indicator < accumulator[m][n]) {
                    max_indicator = accumulator[m][n];
                    m_indicator = m;
                    n_indicator = n;
                }
            }
        }
       circ_x[i]= n_indicator;
       circ_y[i] = m_indicator;
       accumulator[m_indicator][n_indicator]=0;
    }
    Mat output = input.clone();
    cvtColor(output,output,COLOR_GRAY2BGR);
    int max_radius= (int)sqrt((double)(input.rows*input.rows)+(double)(input.cols*input.cols));
    for(int i=0;i<center_num;++i){
        int radius_accumulator[max_radius]={0};
        int max_occurance=0;
        int radius=0;
        for(int m=0;m<input.rows;++m){
            for(int n=0;n<input.cols;++n){
                if(edge_circ.at<uint8_t>(m,n)==0) continue;
                double x_diff = (double)circ_x[i]-(double)n;
                double y_diff = (double)circ_y[i]-(double)m;
                int distance = (int)sqrt(x_diff*x_diff+y_diff*y_diff);
                radius_accumulator[distance]++;
                if(max_occurance<radius_accumulator[distance]){
                    max_occurance = radius_accumulator[distance];
                    radius = distance;
                }
            }
        }
        circle(output,Point(circ_x[i],circ_y[i]),radius,Scalar(255,0,0),1);
    }
    return output;
}

