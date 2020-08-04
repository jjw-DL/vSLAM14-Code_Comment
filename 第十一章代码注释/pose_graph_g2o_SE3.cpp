#include <iostream>
#include <fstream>
#include <string>

#include <g2o/types/slam3d/types_slam3d.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_levenberg.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <g2o/solvers/cholmod/linear_solver_cholmod.h>
using namespace std;

/************************************************
 * 本程序演示如何用g2o solver进行位姿图优化
 * sphere.g2o是人工生成的一个Pose graph，我们来优化它。
 * 尽管可以直接通过load函数读取整个图，但我们还是自己来实现读取代码，以期获得更深刻的理解
 * 这里使用g2o/types/slam3d/中的SE3表示位姿，它实质上是四元数而非李代数.
 * **********************************************/

int main( int argc, char** argv )
{
    //判断命令行个数，这里就一个，就是待优化的位姿数据，也就是pose_graph_g2o_SE3 sphere.g2o文件
    if ( argc != 2 )
    {
        cout<<"Usage: pose_graph_g2o_SE3 sphere.g2o"<<endl;
        return 1;
    }
    //打开文件，同样有防呆，打开失败报警
    ifstream fin( argv[1] );
    if ( !fin )
    {
        cout<<"file "<<argv[1]<<" does not exist."<<endl;
        return 1;
    }

    //第一步：定义矩阵块求解器维度-->第一个6代表要优化的节点的维度，第二个6代表计算误差边的维度
    typedef g2o::BlockSolver<g2o::BlockSolverTraits<6,6>> Block;  // 6x6 BlockSolver
    //第二步：定义矩阵块求解器需要的线性方程求解器
    Block::LinearSolverType* linearSolver = new g2o::LinearSolverCholmod<Block::PoseMatrixType>(); // 线性方程求解器
    //第三步：用线性方程求解器构造块求解器
    Block* solver_ptr = new Block( linearSolver );      // 矩阵块求解器
    // 梯度下降方法，从GN, LM, DogLeg 中选
    //第四步：用块求解器构造下降方法
    g2o::OptimizationAlgorithmLevenberg* solver = new g2o::OptimizationAlgorithmLevenberg( solver_ptr );
    //第五步：构造优化器，将下降方法传入设置。
    g2o::SparseOptimizer optimizer;     // 图模型
    optimizer.setAlgorithm( solver );   // 设置求解器

    int vertexCnt = 0, edgeCnt = 0; // 顶点和边的数量
    while ( !fin.eof() )
    {
        string name;
        fin>>name;
        //按行读取，判断行开头是顶点还是边，依次读入并设置ID，添加进优化器中。
        if ( name == "VERTEX_SE3:QUAT" )
        {
            // SE3 顶点
            g2o::VertexSE3* v = new g2o::VertexSE3();
            int index = 0;
            fin>>index;
            v->setId( index );
            v->read(fin);
            optimizer.addVertex(v);
            vertexCnt++;
            if ( index==0 )
                v->setFixed(true);
        }
        else if ( name=="EDGE_SE3:QUAT" )
        {
            // SE3-SE3 边
            g2o::EdgeSE3* e = new g2o::EdgeSE3();
            int idx1, idx2;     // 关联的两个顶点
            fin>>idx1>>idx2;
            e->setId( edgeCnt++ );
            e->setVertex( 0, optimizer.vertices()[idx1] );
            e->setVertex( 1, optimizer.vertices()[idx2] );
            e->read(fin);
            optimizer.addEdge(e);
        }
        if ( !fin.good() ) break;
    }
    //输出信息：顶点个数、边个数
    cout<<"read total "<<vertexCnt<<" vertices, "<<edgeCnt<<" edges."<<endl;
    
    cout<<"prepare optimizing ..."<<endl;
    // 输出详细优化信息
    optimizer.setVerbose(true);
    optimizer.initializeOptimization();
    cout<<"calling optimizing ..."<<endl;
    //设置优化次数
    optimizer.optimize(30);
    
    cout<<"saving optimization results ..."<<endl;
    //将优化结果保存为.g2o文件
    optimizer.save("result.g2o");

    return 0;
}
