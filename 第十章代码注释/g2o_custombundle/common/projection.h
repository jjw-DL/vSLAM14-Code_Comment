#ifndef PROJECTION_H
#define PROJECTION_H

#include "tools/rotation.h"

/**
 * 这个头文件中只有一个内联函数:带畸变的相机投影
 * inline bool CamProjectionWithDistortion(const T* camera, const T* point, T* predictions){}
 * 整个这个函数就是P246页图10-2的代码实现。
 *
 * @tparam T 很明显要用模板类，以包含float和double这些类型，主要是这两种，int的还没见过
 * @param camera 相机内外参，9维数组。0-2旋转向量、3-5平移向量、6-8相机内参(f,二阶径向畸变系数,四阶径向畸变系数)
 * @param point 投影空间点
 * @param predictions 投影得到的以图像中心为坐标原点下的像素坐标，取名为预测值，因为性质上这个坐标确是估计值。
 * 这里注意一下，这里不是一般意义上像素坐标，一般意义下的像素坐标是以图像左上角为原点。最后面有一些体现～
 * @return 整个投影过程成功的话，返回true。
 */

/*
角轴是一个三维向量，其方向和旋转轴一致，长度等于旋转角。
*/

// camera : 9 dims array with 
// [0-2] : angle-axis rotation 
// [3-5] : translateion
// [6-8] : camera parameter, [6] focal length, [7-8] second and forth order radial distortion
// point : 3D location. 
// predictions : 2D predictions with center of the image plane. 

template<typename T>
inline bool CamProjectionWithDistortion(const T* camera, const T* point, T* predictions){
    // Rodrigues' formula
    //创建一个中间变量，这个p就是相机坐标系下的空间点的坐标。
    T p[3];
    // 通过tool文件夹中的rotation.h中的AngleAxisRotatePoint()函数计算在相机仅旋转的情况下，新坐标系下的坐标。说白就是p=R*point。
    // 这里有个小现象点开此函数定义发现，它的第一个参数需要的是个三元素数组，而这里的camera数组是9元素的，也是能用的。对应只取到前三维。
    // 由数组名的本质可知为什么
    AngleAxisRotatePoint(camera, point, p);
    // camera[3,4,5] are the translation
    // 旋转完之后，平移直接加上就好了。此时的p=R*point+t。即是相机坐标系下空间点的坐标了。也就是式10.36的p`
    p[0] += camera[3]; p[1] += camera[4]; p[2] += camera[5];

    // Compute the center fo distortion
    // 归一化坐标，也就是在求式10.37的Pc，这里的xp yp就是书上的uc vc。
    // 这个负号是转到真实投影平面 简化模型的时候才是成像平面对应到相机前方，不用负号
    T xp = -p[0]/p[2];
    T yp = -p[1]/p[2];

    // Apply second and fourth order radial distortion
    // 这四步将二四阶畸变系数取出来，构造成一个畸变distortion。也就是式10.38中()中的东西
    const T& l1 = camera[7];
    const T& l2 = camera[8];

    //distortion*xp为式10.39中的Uc’和Vc‘，再乘上焦距f得到式10.39中的fx*Uc’和fx*Vc‘。
    //注意，到此为止了，并没有算得整个式10.39中的Us和Vs。
    //也就是predictions的坐标是以图像正中心为原点坐标系下的像素坐标。不是一般意义的以图像左上角为原点开始的坐标。
    //再回头看整个函数，camera参数压根没有传进来Cx和Cy，所以也就无从算起了。也好搞，根据图像大小直接就能算出来一般意义下的像素坐标
    //这里猜测一下为什么只到这里结束了，应该还是为了不同分辨率下图像程序通用性而考虑。
    T r2 = xp*xp + yp*yp;
    T distortion = T(1.0) + r2 * (l1 + l2 * r2);

    const T& focal = camera[6];
    predictions[0] = focal * distortion * xp;
    predictions[1] = focal * distortion * yp;

    return true;
}



#endif // projection.h