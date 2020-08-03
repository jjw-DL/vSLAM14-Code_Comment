#ifndef BUNDLEPARAMS_H
#define BUNDLEPARAMS_H

#include <string>
#include "flags/command_args.h"

using namespace std;

/**
 * 这个结构体定义了Bundle Adjustment使用所有参数。
 * 说一下整体的结构和逻辑：
 * 结构体名称非常直观，就是BA的参数，当然成员就是一堆参数了
 * 那在最后用CommandArgs arg;这么一句，相当于创建了一个参数描述类，
 * 结构成员定义完毕，开始构造函数。
 * 构造函数中首先调用arg这个类中的.param()函数，这个函数有两个功能：
 * 1、给上面定义的每一个参数后面挂一个结构体类型的描述，name、type等等描述
 * 2、给这些参数赋了一些值，也就是默认值了。
 * 然后构造函数又调用了arg这个类中的.parseArgs()函数。这个函数功能就是检查一下命令行中有没有用户自定义的参数值，有的话读进来将默认值覆盖。
 */
struct BundleParams{
public:
    //构造函数，在后面定义，这里只声明。
    BundleParams(int argc, char** argv);
    //析构函数
    virtual ~BundleParams(){};

public:
    //这里的一堆参数才是实打实的BA程序中要用的参数变量！！
    //输入,那个文件被处理
    string input;
    //信赖域策略
    string trust_region_strategy;
    //线性求解器
    string linear_solver;
    //稀疏的线性代数求解器
    string sparse_linear_algebra_library;
    //稠密的线性代数求解器
    string dense_linear_algebra_library;
    //是否使用鲁棒核函数
    bool robustify; // loss function
    // double eta;
    
    //迭代次数
    int num_iterations;
    
    // for making noise
    //噪声发生器
    //随机种子
    int random_seed;
    //旋转sigma
    double rotation_sigma;
    //平移sigma
    double translation_sigma;
    //空间点sigma
    double point_sigma;

    // for point cloud file
    //原始点云
    string initial_ply;
    //优化后点云
    string final_ply;
    //此类在command_args.h中，用于解析用户输入的参数
    //创建一个命令行参数类实例，通过下面的构造函数，对这个实例的成员进行赋值。
    CommandArgs arg;

};

//构造函数定义，注意看，前面还是要加上类名称空间BundleParams::
//这里看见，直接处理的命令行，也就是main()中的第一句：BundleParams params(argc,argv)，用命令行实例化一个BundleParams
 BundleParams::BundleParams(int argc, char** argv)
 {  

    /**
     * 从用法上看出来:(参数名称，参数，参数值，参数描述)
     */
    //这个input就是要被BA的数据集。也就是要被处理的文件名。在这个程序里用的就是data文件夹下的problem-16-22106-pre.txt文件。
    //发现默认值是空字符串，所以这种就是肯定要通过命令行输入进来值的参数。
    arg.param("input", input, "", "file which will be processed");
    arg.param("trust_region_strategy", trust_region_strategy, "levenberg_marquardt",                            
              "Options are: levenberg_marquardt, dogleg.");
    arg.param("linear_solver", linear_solver, "dense_schur",          // iterative schur and cgnr(pcg) leave behind...
              "Options are: sparse_schur, dense_schur");
    
    // arg.param("sparse_linear_algebra_library", sparse_linear_algebra_library, "suite_sparse", "Options are: suite_sparse and cx_sparse.");
    // arg.param("dense_linear_algebra_library", dense_linear_algebra_library, "eigen", "Options are: eigen and lapack.");
    
    
    // arg.param("ordering",ordering,"automatic","Options are: automatic, user.");
    arg.param("robustify", robustify, false, "Use a robust loss function");
    

    // arg.param("num_threads",num_threads,1, "Number of threads.");
    arg.param("num_iterations", num_iterations,20, "Number of iterations.");

    arg.param("rotation_sigma", rotation_sigma, 0.0, "Standard deviation of camera rotation "
              "perturbation.");
    arg.param("translation_sigma", translation_sigma,0.0, "translation perturbation.");
    arg.param("point_sigma",point_sigma,0.0,"Standard deviation of the point "
              "perturbation.");
    arg.param("random_seed", random_seed, 38401,"Random seed used to set the state ");
    arg.param("initial_ply", initial_ply,"initial.ply","Export the BAL file data as a PLY file.");
    arg.param("final_ply", final_ply, "final.ply", "Export the refined BAL file data as a PLY");

    //解析命令行中的参数，看看有没有新值过来，有的话就覆盖。
    arg.parseArgs(argc, argv);
 }

#endif