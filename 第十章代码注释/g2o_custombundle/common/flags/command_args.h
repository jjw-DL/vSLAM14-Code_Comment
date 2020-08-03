
#ifndef G2O_COMMAND_ARGS_H
#define G2O_COMMAND_ARGS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>


/**
 * \brief Command line parsing of argc and argv.
 *
 * Parse the command line to get the program options. Additionally,
 * we can store the config in a file and reload a parameter set from
 * this file.
 */


//该类用于解析用户输入的参数，同时也对程序需要的参数提供默认值及文档说明。
/**
 * 这里整体来说一下这个类：
 * 首先定义了结构体CommandArgument，这个结构体就是参数后面的描述块，其中记录了参数的各种信息。
 * 再看成员函数：
 * parseArgs()用于解析命令行传入的参数自定义值，传入了就将默认值覆盖掉。解析成功返回true。
 *
 * 一堆重载的param()函数，用于往_args[]中添加参数描述块，同时会给参数赋默认值。
 *
 * paramLeftOver()用于往_leftOvers和_leftOversOptional中添加描述块
 *
 * 因为_banner是protected的，所以必须要有getBanner()和setBanner()来读取和设定_banner的值
 *
 * parsedParam()函数用于查看一个参数是否由命令行设置新值并成功解析。也就是查看一个参数描述块中的parsed属性值，
 * 这个属性值只有在读命令行参数时，也就是调用parseArgs()函数时才会被置为true，在用param()设定默认值时是不会被置为true的。
 *
 */
class CommandArgs
{
  public:
  /**
     * 这个结构体就是参数背后链接的关于参数的说明块，有name type等等说明，它跟参数的链接通过data指针进行链接。
     * 整个来看还是外部参数的一个描述结构体，并不会真正的去存储参数。
     *
     * 结构体成员CommandArgument。
     * 这个结构体用于记录参数的特性信息，仅是记录参数信息，并不会真正存储参数和参数值。
     * string name：参数名称
     * string description：参数的一些具体描述
     * int type：参数类型，这里用int值来表示类型，在command_args.cpp中有枚举定义：
     * enum CommandArgumentType
     * {CAT_DOUBLE, CAT_FLOAT, CAT_INT, CAT_STRING, CAT_BOOL, CAT_VECTOR_INT, CAT_VECTOR_DOUBLE};
     * void* data :指向参数的指针。就是这个指针链接外部的参数变量。
     * bool parsed :是否已通过命令行解析性
     * bool optional :是否为可选参数
     */
    struct CommandArgument
    {
      std::string name;
      std::string description;
      int type;
      void* data;
      bool parsed;
      bool optional;
       //描述块的构造函数，提供默认值
      CommandArgument() : name(""), description(""), type(0), data(0), parsed(false), optional(false)
      {}
    };
  public:
    //类构造函数
    CommandArgs();
    //类析构函数
    virtual ~CommandArgs();

    /**
     * parse the command line for the requested parameters.
     * 用于解析命令行传入的参数自定义值，传入了就将默认值覆盖掉。解析成功返回true。
     * @param argc the number of params
     * @param argv the value array
     * @param exitOnError call exit() if the parsing fails
     * @return true, if parsing was correct
     */
    bool parseArgs(int argc, char** argv, bool exitOnError = true);

     /**
     * 这一堆重载的param()函数有几个功能：
     * 首先它需要外给它传一个实际的参数变量，也就是这里的第二个参数，int& p，float& p...这一些。表征它需要链接和赋值的是这个参数
     * 创建一个参数描述结构体，把一些描述写进去
     * 把链接的参数给赋值
     * 把参数描述结构体push进_args[]这个参数描述数组中。
     *
     * 参数列表都差不多：参数名称，链接描述的参数，参数值，参数的描述
     * 参数名称和参数描述都为string类型，主要重载的就是这个链接描述的参数和参数值。用于链接各种类型的参数。
     * 点开各个定义发现：
     * 第一句都是实例化一个CommandArgument结构体的描述块
     * 然后设定描述块中的参数的一些描述
     * 将参数值赋值给参数。
     * 最后将描述快_args.push_back()进描述块数组中
     *
     */

    /** add a bool parameter, if found on the command line, will toggle defValue */
    void param(const std::string& name, bool& p, bool defValue, const std::string& desc);
    /** add a int parameter */
    void param(const std::string& name, int& p, int defValue, const std::string& desc);
    /** add a float parameter */
    void param(const std::string& name, float& p, float defValue, const std::string& desc);
    /** add a float parameter */
    void param(const std::string& name, double& p, double defValue, const std::string& desc);
    /** add a string parameter */
    void param(const std::string& name, std::string& p, const std::string& defValue, const std::string& desc);
    /** add an int vector parameter */
    void param(const std::string& name, std::vector<int>& p, const std::vector<int>& defValue, const std::string& desc);
    /** add an vector of doubles as a parameter */
    void param(const std::string& name, std::vector<double>& p, const std::vector<double>& defValue, const std::string& desc);

    /** add a param wich is specified as a plain argument
     * 指定必须由命令行设定值的普通参数
     * 这个函数原理上跟param()是一样的，也是需要一个实际的参数变量，给其挂靠一个描述块，并将其添加到_leftOvers或_leftOversOptional中
     * 区别就是_leftOvers中的参数必须要由命令行赋自定义的值，不然的话报错。并且再设定时，要对其顺序输入，因为在操作时直接就按需读命令行进行设定值了。
     * _leftOversOptional中的参数就不需要，因为本身属性为可选。同样这里也是，顺序要对齐。
     *
     * */
    void paramLeftOver(const std::string& name, std::string& p, const std::string& defValue, const std::string& desc, bool optional = false);

    /**
     * print the value of all params to an ostream
     */
    //这个函数没有定义
    void printParams(std::ostream& os);

    //! return the banner string用于对_banner成员进行读取和设定
    const std::string& getBanner() const { return _banner; }
    void setBanner(const std::string& banner);

    /**
     * print the help
     */
    void printHelp(std::ostream& os);

    /**
     * returns true, if the param was parsed via the command line
     * 这个函数并不是搜索一个参数是否在参数数组中，而是一个参数的值是否由命令行解析得来，也就是是否为用户自定义的值。
     * 跳到定义处发现是在参数数组中找到参数的话返回参数的parsed属性，
     * 继续观察param()，它工作时并不将parsed属性置为true，而是false。也就是说通过param()函数添加的参数，用这个函数查看，并不会返回true
     * 那在哪里才能看见parsed属性被置为true呢，只有在parseArgs()定义内才有被置为true的机会，所以说明了一个参数值是否由命令行成功解析
     * 进而也就表征了一个参数是否被用户自定义了，并没有用默认值。
     */
    bool parsedParam(const std::string& paramFlag) const;

  protected:
  //这里最终将参数的描述结构体保存起来，分别保存为_args，_leftOvers和_leftOversOptional
    /**
     * 这里说一下概况和设计思路。
     * _args[]为参数描述数组，往这个数组里面添加元素用param()，
     * 例如在BundleParams.h中就用param()添加了一些。
     * 如果在程序执行的命令行中又提供了新的值，那么通过parseArgs()函数找到对应的参数，并将默认值覆盖。如果没提供，就使用默认值。
     *
     *
     * _leftOvers[]为其他类型的参数数组，这个数组是通过paramLeftOver()函数来写进数据，
     * 也就是什么参数要事先用paramLeftOver()函数push进去(本程序貌似没用到)，
     * 要求在程序执行命令行中必须提供此类参数的值，不然报错！
     * _leftOversOptional为事先定义好的可选的参数数组，也是通过paramLeftOver()函数添加元素，
     * 与_leftOvers[]不同的是，它不强制命令行必须要提供参数值，命令行提供值与否不重要，可有可无。
     * 做法就是在命令行提供了所有的_leftOvers[]参数值后，剩下的就都push进_leftOversOptional[]里了。
     *
     * 总结一下就是：
     * 提供了默认值的参数放到_args，命令行提供新值就覆盖，不提供就用默认值
     * _leftOvers是必须要命令行提供值的参数。而且还不能一个不能少的要提供。
     * _leftOversOptional是存放那些可选参数的，命令行提不提供都行。
     */
    std::vector<CommandArgument> _args;
    std::vector<CommandArgument> _leftOvers;
    std::vector<CommandArgument> _leftOversOptional;
    //参数描述集标示，被BundleParams结构体纳入为成员后，也是能表征参数集的。
    std::string _banner;
    //程序名称，表征对应哪个程序的参数。赋值方式比较直接暴力，直接将argv[0]赋值给它。
    std::string _progName;

    //一些用到的小功能函数，不细说了，参看定义实现方式。
    //类型转化成字符串，例如将int转换成"int"字符串，很明显就是输出显示用的。
    const char* type2str(int t) const;
    //字符串转化成参数
    void str2arg(const std::string& input, CommandArgument& ca) const;
    //参数转化成字符串
    std::string arg2str(const CommandArgument& ca) const;
    //字符串修剪，将字符串中的纯字符提取出来，把前后的空格，缩进，换行都给剔除掉。
    std::string trim(const std::string& s) const;
};


#endif
