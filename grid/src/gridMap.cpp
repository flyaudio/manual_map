

#include "ros/ros.h" 
#include "sensor_msgs/LaserScan.h" 
#include "nav_msgs/OccupancyGrid.h" 
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <cv_bridge/cv_bridge.h>

 
using namespace cv;
using namespace std;
 
/***************** Mat转vector **********************/
template<typename _Tp>
vector<_Tp> convertMat2Vector(const Mat &mat)
{
	return (vector<_Tp>)(mat.reshape(1, 1));//通道数不变，按行转为一行
}
 
/****************** vector转Mat *********************/
template<typename _Tp>
cv::Mat convertVector2Mat(vector<_Tp> v, int channels, int rows)
{
	cv::Mat mat = cv::Mat(v);//将vector变成单列的mat
	cv::Mat dest = mat.reshape(channels, rows).clone();//PS：必须clone()一份，否则返回出错
	return dest;
}


int main(int argc, char * argv[]) 
{ 
    ros::init(argc, argv, "gridMap");
    
    ros::NodeHandle nh; 
    ros::NodeHandle nh_private("~");
    
    ros::Publisher pub = nh.advertise<nav_msgs::OccupancyGrid>("/gridMap", 1); 
    nav_msgs::OccupancyGrid map; 
    
    map.header.frame_id="grid"; 
    map.header.stamp = ros::Time::now(); 
    map.info.resolution = 0.02; // float32 
    map.info.width = 640; // uint32 
    map.info.height = 640; // uint32 

    // the origin of the map[m,m,rad]. This's the real-world pose of the cell(0,0) in the map
    map.info.origin.position.x = -3;//
    map.info.origin.position.y = -9.78;//
    
    int p[map.info.width*map.info.height] = {-1}; // [0,100] 
    p[0] = 100; 
    p[5-1] = 100; 
    std::vector<signed char> a(p, p + map.info.width*map.info.height); 
    map.data = a; 
    
    float resolution = 2.0 ;//cm/pixel
    float length = 554.0 ;//cm
    float width = 498.0 ;//cm
    float LaneWidth = 120.0 ;//cm
    cv::Point p1(150,150); //top left point
    cv::Point p2(p1.x+int(length/resolution)-1,p1.y);
    cv::Point p3(p1.x+int(length/resolution)-1,p1.y+int(width/resolution)-1);
    cv::Point p4(p1.x,p1.y+int(width/resolution)-1);
    cv::Mat MapImg(map.info.height, map.info.width, CV_8UC1, cv::Scalar(205));
    cv::line(MapImg, p1, p2, cv::Scalar(255), 1, 4);//CV_AA
    cv::line(MapImg, p2, p3, cv::Scalar(255), 1, 4);//CV_AA
    cv::line(MapImg, p3, p4, cv::Scalar(255), 1, 4);//CV_AA
    cv::line(MapImg, p4, p1, cv::Scalar(255), 1, 4);//CV_AA,4,8


    cv::imshow("test", MapImg);

    cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(int(LaneWidth/resolution), int(LaneWidth/resolution)));
	cv::Mat result;
	cv::dilate(MapImg, result, element);

    cv::Point edgeP11(p1.x - int(LaneWidth/2/resolution)+1, p1.y - int(LaneWidth/2/resolution)+1);//top left point
    cv::Point edgeP12(edgeP11.x + int((length + LaneWidth)/resolution), edgeP11.y);
    cv::Point edgeP13(edgeP11.x + int((length + LaneWidth)/resolution), edgeP11.y+int((width + LaneWidth)/resolution));
    cv::Point edgeP14(edgeP11.x, edgeP11.y+int((width + LaneWidth)/resolution));
    cv::line(result, edgeP11, edgeP12, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP12, edgeP13, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP13, edgeP14, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP14, edgeP11, cv::Scalar(0), 1, 4);//CV_AA,4,8

    cv::Point edgeP21(p1.x + int(LaneWidth/2/resolution), p1.y + int(LaneWidth/2/resolution));//top left point
    cv::Point edgeP22(edgeP21.x + int((length - LaneWidth)/resolution), edgeP21.y);
    cv::Point edgeP23(edgeP21.x + int((length - LaneWidth)/resolution), edgeP21.y + int((width - LaneWidth)/resolution));
    cv::Point edgeP24(edgeP21.x, edgeP21.y + int((width - LaneWidth)/resolution));
    cv::line(result, edgeP21, edgeP22, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP22, edgeP23, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP23, edgeP24, cv::Scalar(0), 1, 4);//CV_AA
    cv::line(result, edgeP24, edgeP21, cv::Scalar(0), 1, 4);//CV_AA,4,8

    cv::putText(result, "door", cv::Point(249,466), cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(255), 1, CV_AA);
    cv::putText(result, "starting point(150,150) in pixel", cv::Point(125,107), cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(255), 1, CV_AA);
    // cv::circle(result, p1, 1, Scalar(0), -1);  // 画半径为1的圆(画点）
    cv::imwrite("map.pgm", result);
	imshow("result", result);
    cv::waitKey(0);
    // dstMap = convertVector2Mat<unsigned char>(a, 1, map.info.height);
    // cout << "=======\n" << MapImg  << endl;
    // cout << "=======\n" << MapImg.at<uchar>(0,0)  << endl;

    // int nr=MapImg.rows;  
    // // 将3通道转换为1通道  
    // int nl=MapImg.cols*MapImg.channels();  
    // for(int k=0;k<nr;k++)  
    // {  
    //     // 每一行图像的指针  
    //     const uchar* inData=MapImg.ptr<uchar>(k);  
    //     for(int i=0; i<nl; i++)  
    //     {  
    //         cout << inData[i] << endl;
    //     }  
    // } 




    // for(int i=0; i<dstMap.rows; i++)  
    // {  
    //     for(int j=0; j<dstMap.cols; j++)  
    //     {  
    //         cout << dstMap.at<uchar>(i,j) << " " ;
    //     }  
    // }
    
    // std::vector<unsigned char> b = convertMat2Vector<unsigned char>(dstMap);
    // for (int i=0; i<a.size(); i++)
    // {
    //     cout << a.at(i) << "-";
    // }
// ==================
    // std::vector<unsigned char> t1;
    // if(dstMap.isContinuous())
    // {
    //     t1.assign(dstMap.data, dstMap.data + dstMap.total()) ;
    //     cout <<"type = " << dstMap.type() << endl;
    //     cout << "CV_8UC1 = " << CV_8UC1 <<endl; 
    // }
    // else
    // {
    //     cout << "not continuous" << endl;
    // }
    // for(auto iter = t1.begin(); iter != t1.end(); iter++)
    // {
    //     std::cout << (*iter) <<std::endl;
    // }

// ==================


    // // uchar arr[4][3] = { { 1,2,3 },{ 4,5,6 },{ 7,8,9 },{ 10,11,12 } };
    // uchar arr[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
	// cv::Mat srcData(1, 12, CV_8UC1, arr);
	// cout << "srcData=\n" << srcData << endl;
 
 
	// vector<uchar> v = convertMat2Vector<uchar>(srcData);
    // for (int i=0; i<v.size(); i++)
    // {
    //     cout << v.at(i) << "-";
    // }	
    // cv::Mat dest = convertVector2Mat<uchar>(v, 1, 1);//把数据转为1通道，4行的Mat数据
	// cout << "dest=\n" << dest << endl;


    while (ros::ok()) 
    { 
        pub.publish(map); 
    } 
    
    ros::shutdown(); 
    return 0; 
}

