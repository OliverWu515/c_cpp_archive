# dip_hw3

大三数字图像处理（Digital Image Processing）课程的第三次作业，使用 C++ 和 OpenCV 实现边缘检测与霍夫圆检测。

- `CMakeLists.txt`：原 ROS Catkin 工程的构建配置，声明了 `roscpp`、`std_msgs` 和 OpenCV 依赖，并生成名为 `hough` 的可执行文件。
- `package.xml`：Catkin 软件包清单，记录包名、版本及构建工具等元数据。
- `data/circle.png`：包含圆形与方形轮廓的测试图，用于霍夫圆检测。
- `data/lena.bmp`：经典 Lena 测试图，用于对比 Sobel、Roberts、Prewitt、LoG、自实现 Canny 与 OpenCV Canny 的边缘检测效果。
- `src/hw3_hough.cpp`：调整后的作业代码。包含二维卷积、高斯核生成、图像二值化，以及 Sobel、Roberts、Prewitt、LoG 和 Canny 边缘检测；随后利用梯度方向投票寻找圆心，并统计半径以绘制检测到的圆。该版本跳过了 Canny 阶段的高斯平滑，并将双阈值调整为 `80/50`，和 OpenCV 版本取得了接近的效果。
- `src/hw3_hough_original.cpp`：原始版本，用于保留修改前的实现。与调整版相比，它在 Canny 检测前执行高斯平滑，双阈值为 `25/10`。

**运行提示**

测试图像现已收录在 `dip_hw3/data/` 中。源码仍使用原 Catkin 工作空间下的相对路径：

```text
./src/HW3/data/circle.png
./src/HW3/data/lena.bmp
```

如需保持源码不变，应将 `dip_hw3` 作为 `HW3` 软件包放到 Catkin 工作空间的 `src/` 目录中，并从工作空间根目录运行程序，使上述相对路径能够正确解析。也可以直接将源码中的图片路径改为当前实际位置。编译需要 ROS Catkin、CMake 与 OpenCV 开发环境。