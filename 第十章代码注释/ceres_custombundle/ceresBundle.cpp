#include <iostream>
#include <fstream>
#include "ceres/ceres.h"

#include "SnavelyReprojectionError.h"
#include "common/BALProblem.h"
#include "common/BundleParams.h"


using namespace ceres;

void SetLinearSolver(ceres::Solver::Options* options, const BundleParams& params)
{
    //linear solver选取
    CHECK(ceres::StringToLinearSolverType(params.linear_solver, &options->linear_solver_type));
    CHECK(ceres::StringToSparseLinearAlgebraLibraryType(params.sparse_linear_algebra_library, &options->sparse_linear_algebra_library_type));
    CHECK(ceres::StringToDenseLinearAlgebraLibraryType(params.dense_linear_algebra_library, &options->dense_linear_algebra_library_type));
    // 设置线程数量
    options->num_linear_solver_threads = params.num_threads;

}

//消元顺序设置
void SetOrdering(BALProblem* bal_problem, ceres::Solver::Options* options, const BundleParams& params)
{
    const int num_points = bal_problem->num_points();
    const int point_block_size = bal_problem->point_block_size();
    double* points = bal_problem->mutable_points();

    const int num_cameras = bal_problem->num_cameras();
    const int camera_block_size = bal_problem->camera_block_size();
    double* cameras = bal_problem->mutable_cameras();

    //这里如果设置为自动话直接return
    if (params.ordering == "automatic")
        return;

    //创建个ParameterBlockOrdering类型的对象，在下面按顺序把参数码进去，达到排序的目的，进行按序消元
    ceres::ParameterBlockOrdering* ordering = new ceres::ParameterBlockOrdering;

    // The points come before the cameras  将points和camera分组，优先消元编号小的变量
    for(int i = 0; i < num_points; ++i)
       ordering->AddElementToGroup(points + point_block_size * i, 0);
       
    
    for(int i = 0; i < num_cameras; ++i)
        ordering->AddElementToGroup(cameras + camera_block_size * i, 1);
    
    //最后就是用这句设置消元顺序
    options->linear_solver_ordering.reset(ordering);

}

void SetMinimizerOptions(Solver::Options* options, const BundleParams& params){
    //最大迭代次数：
    options->max_num_iterations = params.num_iterations;
    //标准输出端输出优化log日志
    options->minimizer_progress_to_stdout = true;
    //设置线程，加速雅克比矩阵计算
    options->num_threads = params.num_threads;
    // options->eta = params.eta;
    // options->max_solver_time_in_seconds = params.max_solver_time;
    
    //下降策略选取
    CHECK(StringToTrustRegionStrategyType(params.trust_region_strategy,
                                        &options->trust_region_strategy_type));
    
}

void SetSolverOptionsFromFlags(BALProblem* bal_problem,
                               const BundleParams& params, Solver::Options* options){
    //这里其实可以堆一堆的优化选项设置，也就是列一堆options->，只不过根据设置的功能，又单分成了三个函数
    //书上P267的求解设置就是一堆options->
    //这里也可以发现，ceres的设置是比较简单的，定义个option对象，直接一通options->就好了。
    SetMinimizerOptions(options,params);
    SetLinearSolver(options,params);
    SetOrdering(bal_problem, options,params);
}

/**
 * 构建问题，主要是将优化数据和传入problem，添加残差
 * @param bal_problem 数据
 * @param problem 要构建的优化问题，感觉ceres中的problem就类似于g2o中的optimizer，就是构建出优化问题。
 * @param params 优化所需参数
 */
void BuildProblem(BALProblem* bal_problem, Problem* problem, const BundleParams& params)
{
    //读出路标和相机的维度
    const int point_block_size = bal_problem->point_block_size();
    const int camera_block_size = bal_problem->camera_block_size();
    //获取路标和相机的数据首位置
    double* points = bal_problem->mutable_points();
    double* cameras = bal_problem->mutable_cameras();

    // Observations is 2 * num_observations long array observations
    // [u_1, u_2, ... u_n], where each u_i is two dimensional, the x 
    // and y position of the observation. 
    //获取观测数据首位置
    const double* observations = bal_problem->observations();

    for(int i = 0; i < bal_problem->num_observations(); ++i){
        CostFunction* cost_function;

        // Each Residual block takes a point and a camera as input 
        // and outputs a 2 dimensional Residual
        // 定义problem->AddResidualBlock()函数中需要的cost_function  ！！！！有导数性质
        cost_function = SnavelyReprojectionError::Create(observations[2*i + 0], observations[2*i + 1]);

        // If enabled use Huber's loss function. 
        // 定义problem->AddResidualBlock()函数中需要的Huber核函数
        LossFunction* loss_function = params.robustify ? new HuberLoss(1.0) : NULL;

        // Each observatoin corresponds to a pair of a camera and a point 
        // which are identified by camera_index()[i] and point_index()[i]
        // respectively.
        // 定义problem->AddResidualBlock()函数中需要的待估计参数
        double* camera = cameras + camera_block_size * bal_problem->camera_index()[i];
        double* point = points + point_block_size * bal_problem->point_index()[i];

        //往问题中添加误差项：
        /**
         * 看一下函数定义：
         * cost_function： 中文怎么称呼，代价函数?
         * 多说几句这个cost_function，既不是误差，因为他是误差构造出来的。用法上瞅着有点像导数的意味，类似于g2o的雅克比矩阵？
         * loss_function： 损失函数，就是核函数
         * 紧接着的数组就是待优化参数了，由于各参数维度不同，所以类型为double*,有几个就写几个，这里两个，camera和point
         * ResidualBlockId AddResidualBlock(CostFunction* cost_function,
                                   LossFunction* loss_function,
                                   double* x0, double* x1);
         */
        problem->AddResidualBlock(cost_function, loss_function, camera, point);
    }

}

//这个函数就是整个优化问题被单拉了出来。参数也很原始：待优化数据和优化参数
void SolveProblem(const char* filename, const BundleParams& params)
{
    //同样一开始，用BALProblem类对数据进行处理
    BALProblem bal_problem(filename);

    // show some information here ...
    std::cout << "bal problem file loaded..." << std::endl;
    std::cout << "bal problem have " << bal_problem.num_cameras() << " cameras and "
              << bal_problem.num_points() << " points. " << std::endl;
    std::cout << "Forming " << bal_problem.num_observations() << " observatoins. " << std::endl;

    // store the initial 3D cloud points and camera pose..
    if(!params.initial_ply.empty()){
        bal_problem.WriteToPLYFile(params.initial_ply);
    }

    std::cout << "beginning problem..." << std::endl;
    
    // add some noise for the intial value
    srand(params.random_seed);
    bal_problem.Normalize();
    bal_problem.Perturb(params.rotation_sigma, params.translation_sigma,
                        params.point_sigma);

    std::cout << "Normalization complete..." << std::endl;
    
    //构建最小二乘问题，problem其实就是目标函数
    Problem problem;
    BuildProblem(&bal_problem, &problem, params);//向问题中添加所有观测的残差

    std::cout << "the problem is successfully build.." << std::endl;
   
    //优化选项设置
    Solver::Options options;
    SetSolverOptionsFromFlags(&bal_problem, params, &options);
    options.gradient_tolerance = 1e-16;
    options.function_tolerance = 1e-16;
    //summary输出优化简报
    Solver::Summary summary;
    //真正的优化就这一句：Solve()函数，传入选项设置、目标函数、简报输出变量。
    Solve(options, &problem, &summary);
    std::cout << summary.FullReport() << "\n";

    // write the result into a .ply file.   
    if(!params.final_ply.empty()){
        bal_problem.WriteToPLYFile(params.final_ply);  // pay attention to this: ceres doesn't copy the value into optimizer, but implement on raw data! 
    }
}

int main(int argc, char** argv)
{    
    BundleParams params(argc,argv);  // set the parameters here.
   
    google::InitGoogleLogging(argv[0]);
    std::cout << params.input << std::endl;
    if(params.input.empty()){
        std::cout << "Usage: bundle_adjuster -input <path for dataset>";
        return 1;
    }

    SolveProblem(params.input.c_str(), params);
 
    return 0;
}