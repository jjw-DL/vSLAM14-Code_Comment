
#include "command_args.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <functional>


using namespace std;


// forward declarations
//四个重载，用于输入输出int型数组和double型数组,这个纯粹就是功能，跟BA程序没关系。
std::istream& operator>>(std::istream& is, std::vector<int>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<int>& v);
std::istream& operator>>(std::istream& is, std::vector<double>& v);
std::ostream& operator<<(std::ostream& os, const std::vector<double>& v);

/**
 * 重载输入符>>，让其能够给int类型数组进行输入。
 * 参数很明显就是两个了，输入流对象引用和要被输入的数组
 */
std::istream& operator>>(std::istream& is, std::vector<int>& v){
  
  // 创建一个字符串s，用于承接输入流is中的数据
  string s;
  // 如果输入失败，则直接return了。这里主要用处还是防呆。
  // 为什么这里可以用>>呢，因为输入流到字符串是已经定义好的，流中的数据本来就是一律视为字符的，所以直接输入到字符串是没毛病的。
  if (! (is >> s) )
    return is;
  /**
   * 如果能走到这里，说明没有在上一步return，说明is >> s成功了，说明输入流中的数据已经存进了字符串s中。
   * 那么下面就是将字符串s中的数字解析出来，存进int型数组v中。
   */

  //.c_str()函数返回字符串的首字符地址。返回类型为const char*，所以不能直接赋值给char*类型
  const char* c = s.c_str();
  //这一步用了一下类型转换，将const char*类型的c,转换成char*类型，并赋值给char*类型的caux。
  //最后要的结果就是： caux 指向了字符串s的首字符的地址。
  char* caux = const_cast<char*>(c);

  //首先将数组v清空。
  v.clear();
  //定义一个循环遍历控制标识符，bool型
  bool hasNextValue=true;
  //通过hasNextValue控制循环解析
  while(hasNextValue){
    /**
     * string to long int 函数说明
     * 函数定义: long int strtol(const char *nptr, char **endptr, int base)
     * 函数说明: 将nptr字符串转换成长整型数
     * 参数说明：
     * const char *nptr，要被转换的字符串
     * char **endptr，转换到哪个字符不符合条件结束了，将指向这个字符的指针的地址反馈给endptr。这里发现endptr是char**类型的。
     * base，指转换成几进制的数。base范围从2至36，或0。
     * 如base值为10则采用10进制(字符串以10进制表示)，若base值为16则采用16进制(字符串以16进制表示)。
     * 当base值为0时则是采用10进制做转换，但遇到如''0x''前置字符则会使用16进制做转换。
     */

    /**
     * 指针c用于指向解析的开头处，指针caux用于指向解析的结尾处，
     */
    // 这里开始解析转换：
    // 从c指向的地方开始解析，
    // 将一次解析结束的位置，赋值给caux，
    // 按照10进制解析
    // 解析出来的返回类型为long int，所以进行下类型转换，转换成int型，并赋值给i。
    // 这句完之后，一次解析就完成了，解析出来的数字存在了i中。c指向开头处，caux指向解析掉i后的位置。
    int i = static_cast<int>(strtol(c,&caux,10));

    //下面这里判断一下指向开头的c和指向结束的caux是否指向了同一个位置，如果指向同一个位置，说明已经解析完了。
    //因为如果解析完了的话，c开头解析，没解析就停止了，导致caux指向的就是c开头的位置，说明解析完了。
    //如果没解析完，进行一些正常的操作：
    if (c!=caux){
      //将末尾指针caux的值赋值给开头指针c，
      c=caux;
      //跳出的位置肯定不是数字了，所以从下一个字符开始
      c++;
      //然后就是最重要的，将解析出来的数字，push进数组v中。
      v.push_back(i);
    } else
      //else说明解析完了，将循环判断标志置为false，然后循环就跳出了。
      hasNextValue = false;
  }
  //当然最后，为了能够链接式的使用>>,最后肯定要返回输入流对象引用，同理上面判断失败直接return，也是返回的is。
  return is;
}

/**
 * 重载输出符<<，让其能够对int类型数组进行输出。
 * @param os 输出流对象引用
 * @param v 要被输出的int数组
 * @return 因为要保证链式使用，所以返回输出流对象引用
 */
std::ostream& operator<<(std::ostream& os, const std::vector<int>& v){
  //将第一个元素单独输出，因为后面的循环看出来，元素前面有个"," 而第一个是不需要的，所以单独输出。
  //同时判断一下，v是否为空。
  if (v.size()){
    os << v[0];
  }
  //剩下的就是for循环进行输出了，从第二个元素开始，直到最后一个。
  for (size_t i=1; i<v.size(); i++){
    os << "," << v[i];
  }
  //最后返回输出流对象引用
  return os;
}


/**
 * convert a string into an other type.
 * 将字符串转化成其他类型，一般是纯数字字符串转化成int、double、float值等这种用法。明显要用模板。
 *
 * 原理还是利用字符串流，先构造字符串输入流i，再将其作为输入，输入到x中。
 * 返回值返回转化成功与否
 */
template<typename T>
bool convertString(const std::string& s, T& x)
{
  //这里用s字符串构造了一个输入流对象i，
  std::istringstream i(s);
  //将流输入进x,同时判断输入是否成功。
  if (! (i >> x))
    return false;
  return true;
}

/** Helper class to sort pair based on first elem 
 * 基于pair中的第一个元素对类进行排序
*/
template<class T1, class T2, class Pred = std::less<T1> >
struct CmpPairFirst {
  bool operator()(const std::pair<T1,T2>& left, const std::pair<T1,T2>& right) {
    return Pred()(left.first, right.first);
  }
};
//这里就是参数类型的枚举，用不同的int值来表征类型，也就是为什么commad_args中的type属性的类型是int型的了。
enum CommandArgumentType
{
  CAT_DOUBLE, CAT_FLOAT, CAT_INT, CAT_STRING, CAT_BOOL, CAT_VECTOR_INT, CAT_VECTOR_DOUBLE
};
//class CommandArgs的默认构造函数
CommandArgs::CommandArgs()
{
}
//class CommandArgs的默认析构函数
CommandArgs::~CommandArgs()
{
}

/**
 * 整个的命令行用法应该是：
 * ./progname -parameter1 parameter1_value -parameter2 parameter2_value -- parameterleftover...
 *
 * 整体的逻辑是程序名后面接一个-参数名，参数名后面接参数值，成对出现，在后面会有--或者直接是参数名的，这些是放到_leftOvers[]中的。
 * 对于-参数，就去_args[]数组中去寻找，找到后将自定义的值覆盖进去。没找到肯定报错。没找到说明输入了不相关的参数。
 * 对于_leftOvers[]中的参数，就依次读进去值，在_leftOvers[]读满了之后，将后面的放在_leftOversOptional[]中。
 */
bool CommandArgs::parseArgs(int argc, char** argv, bool exitOnError)
{
  //第一个命令行参数为程序名称，将其保存在_progName
  _progName = argv[0];

  int i;
  //用argc控制循环遍历命令行参数数组
  for (i = 1; i < argc; i++) {

    //这里将第i个参数赋值给name字符串。
    string name = argv[i];

    //每个命令参数的参数名必须以至少一个‘-’字符开头，多个也没问题，不是的话直接break到后面对于_leftOvers[]的输入
    if (name[0] != '-') { // each param has to start with at least one dash
      //cerr << "Error: expecting parameter, got " << name << endl;
      //printHelp(cerr);
      //if (exitOnError)
        //exit(1);
      //return false;
      break;
    }
    /* first check whether it's -- and we should not continue parsing */
    //首先检查是否是'--',表征人为自定义值输入结束，也是break到后面的_leftOvers[]的输入
    if (name == "--") {
      //这里++i是因为本字符已经被检查出是"--"了，再读取就没什么意思了，所以从后开始。
      //对比上面的break，上面的只是检测出来一个命令行参数不是以'-'开头，所以有可能就是_leftOvers[]的输入了，所以不++i
      ++i;
      break;
    }
    
    //上面过滤掉了没有'-'和‘--’的情况，那么剩下的就是‘-’和‘----’的情况，找到第一个不是'-'的地方，然后将前面的切掉。重新赋值name
    string::size_type dashPos = name.find_first_not_of('-');
    //判断一下，防止只有横杠的情况。
    if (dashPos != string::npos)
      //剪切掉dashPos之前的东西，只留后面的，重新赋值name，此时的name就是干净的参数名了。
      name = name.substr(dashPos);

    //判断剩下的name字符串是否是”help“或者”h“，是的话就打印帮助然后，exit()
    if (name == "help" || name == "h") {
      printHelp(cout);
      exit(0);
    }
    else {
      // command line argument parsing
      //排除掉上面一切其他情况，这里开始正常的参数赋值。
      //这里创建了一个迭代器：std::vector<CommandArgument>::iterator类型的it，指向_args的开始处。
      std::vector<CommandArgument>::iterator it = _args.begin();
      //迭代器开始在_args中找相应的参数。
      for ( ; it != _args.end(); ++it) {
        //如果_args数组中找到了命令行中对应的参数名
        if (it->name == name) {
          //找到有此参数之后，看类型是否是bool型的，如果是，操作上就是直接翻转
          if (it->type == CAT_BOOL) {
            //判断一下是否没有读取，没读取执行下面的操作：对于读取过了的情况发现就直接不管了。不进行任何操作。
            if (!it->parsed) {
              //同样的套路，将参数取出进行赋值就好了，由于这里是bool类型，所以直接拉出来翻转即可。
              bool* data = static_cast<bool*>(it->data);
              *data = !(*data);
            }
            //操作完之后，将其parsed属性置为true。表征命令行读入参数值成功
            it->parsed = true;
          } else {
            /**
             * 这一步的意义是检查一下命令行是否只给了参数名，而后面结束了，并没有给参数的值。
             * 说一下原理：整个程序有两处对i的值进行操作，一处就是for循环中的i++,这里记为第一个i++
             * 另外一处就是在str2arg(argv[i], *it);读入参数值之前的一句i++,这里记为第二个i++
             * 也就是说，从第一个i++之后到第二个i++之前，argv[i]都是参数名。
             * 第二个i++之后到第一个i++之前，argv[i]都是参数值。
             * 很明显，这一句的位置在第一个i++与第二个i++之间。argv[i]应该是一个参数名。
             * 而i >= argc-1是什么意思呢？
             * 先看i = argc-1的情况：i在这里的作用就是用于argv[i]，用于遍历命令行的各个命令的。
             * 那么argv[argc-1]是哪个命令呢，由于argv的索引从0开始，而argc为个数计数，是从1开始的，所以argv[argc-1]就是只的是最后一个命令行参数
             * 问题来了，此处的argv[i]是一个参数名，后面是需要有参数值的，而这里判断这个参数已经为最后一个命令了，后面没有命令提供参数值了。
             * 所以面的一系列报警输出，exit()等就好理解了。
             * 至于i>argc-1的情况就更好理解了。
             * 举个例子，假如命令行就提供了一个命令参数，此时argc=2，argv也就只有argv[0]和argv[1],
             * 你非得去读取argv[2]或者argv[3]等等越界的地方，肯定结果也是输出报错。
             */
            if(i >= argc-1) {
              cerr << "Argument " << name << "needs value.\n";
              printHelp(cerr);
              if (exitOnError)
                exit(1);
              return false;
            }
            //紧接着，跟在读参数名后面的参数值，所以i++，让argv[i]对应参数值。
            i++;
            // 由于命令行类型一律是字符串，所以用到了定义的字符串转参数的函数，
            // 看函数定义发现其实就是用字符串格式的参数值给参数赋值。
            str2arg(argv[i], *it);
            //解析完成，将parsed置为true
            it->parsed = true;
          }
          /**
         * 这个break想了好久。。还是程序结构没看懂。
         * for ( ; it != _args.end(); ++it){}中是在拿着命令行的参数名(这里的name)去_args[]中去寻找对应的参数，找到了将值更新。
         * 这个for{}中只有一个if (it->name == name){}，也就是说只定义了找到的情况，找到后自然就是赋值，parsed置为true等操作
         * 问题就在这，赋值完了，parsed也置为true了之后呢，之后程序怎么走？
         * 仔细观察你就发现会自然的会走到这一句break，跳出这个for ( ; it != _args.end(); ++it){}遍历_args[]的循环。
         * 跳出之后， 如果下面的if (it == _args.end()){}不执行，那就正常跑到了for (i = 1; i < argc; i++){}的一次循环的末尾，
         * 也就是// for argv[i]这一句的那个括号。去之后就是再读一个argv[i]，再去遍历_args[],找到赋值，找不到报警。如此循环。
         *
         */
          break;
        }
      }
      //如果it被++到了_args末尾后一个，说明命令行中输入的参数名，并没有在_args[]数组中找到，也就是说命令行中输入了一个不相干的参数名，
      //所以也是输出报警信息，并退出，返回false等一些列操作。
      /**
       * 这一句也是因上面的break才可以这么写。上面找到对应的变量并赋值成功后会break，说明it会在_args[]中间的一个位置。
       * 所以成功的话这个if永远不会成立。
       * 成立的情况是什么样的呢？it在遍历 _args[]时，遍历到末尾遍历完了也没找到。
       * 也就是说上方的for ( ; it != _args.end(); ++it){}都完整运行完了，也没找到。
       * 那就说明你命令行输入的参数名我这个程序并不认识。输入了一个不相干的参数。所以报警输出为unknown
       */
      if (it == _args.end()) {
        cerr << "Error: Unknown Option '" << name << "' (use -help to get list of options).\n";
        if (exitOnError)
          exit(1);
        return false;
      }

    }

  } // for argv[i]

  /**
     * 这里是承接上面判断命令行的参数开头没有-或者为--的。两个break到了这里。
     * ‘--’有i++，argv[i]跳到了下一个字符串，而没有-的也就没有i++，说明argv[i]就在brake处的当前字符串。
     * 这里的判断是说剩余的命令行参数够不够填满_leftOvers[]了，而这里面的参数必须要用户定义值，如果不够了，说明没有提供足够的必要参数值。
     * 所以这里就cerr报警说缺参数值，打印帮助，程序退出，返回false等一系列的操作。
     * 如果这一步不满足，说明剩下的命令行参数还够填满_leftOvers[]数组，填满了多的就继续放在_leftOversOptional[]中
     */
  if ((int)_leftOvers.size() > argc - i) {
    cerr << "Error: program requires parameters" << endl;
    printHelp(cerr);
    if (exitOnError)
      exit(1);
    return false;
  }
  // 这里其他参数的命令行赋值，就必须要对其顺序了。一般都会先将必须要有的_leftOvers[]赋全，然后触发j < _leftOvers.size()不满足，循环结束
  // 因为不检查名字，按顺序赋值给_leftOvers[j],所以顺序一定要对齐。
  // 这里说一下这两个循环控制符i和j，由于上方的if已经判定了，所以一定是j < _leftOvers.size()先触发完毕，也就是赋全了_leftOvers[].
  for (size_t j = 0; (i < argc && j < _leftOvers.size()); i++, j++) {
    // 这两步捯饬还是将命令行中的值，赋值给内部的参数。
    // 一步一步来看，_leftOvers[j].data是描述块所描述的参数变量的地址，将这个地址赋值给s
    string* s = static_cast<string*>(_leftOvers[j].data);
    //将s所指地址上的数据，赋值为argv[i]，s就是指的描述块所描述的参数变量。所以参数变量的值被修改
    *s = argv[i];
    // 这里来看，还是一步比较容易理解：将.data所指的地方的值，更新为argv[i]
    //*(static_cast<string*>(_leftOvers[j].data))=argv[i];
  }

  // the optional leftOvers
  // 然后就是这里了，将填满_leftOvers后剩下的命令行参数值放到_leftOversOptional中。由于不检查名字，也是按顺序直接赋值,所以顺序一定要对齐。
  // 这里的i，j判定条件谁先达到就不知道了。i先到头，说明_leftOversOptional[]没有赋全，可选，没关系。
  // j先到头，说明_leftOversOptional[]也赋全了，所有的参数都已经被自定义了值，后面剩下的那些命令行参数就扔了，不要了。
  for (size_t j = 0; (i < argc && j < _leftOversOptional.size()); i++, j++) {
    string* s = static_cast<string*>(_leftOversOptional[j].data);
    *s  = argv[i];
  }
  //能走到这一步，说明正确解析完成，没有在中间报错退出，所以可以返回true了。
  return true;
}

/**
 * 下面一堆重载的param()函数，用于往_args[]数组中添加参数描述块。
 * @param name 参数名称
 * @param p 参数快链接的参数。这个就是BA要用的内部参数。
 * @param defValue 参数值
 * @param desc 参数描述
 */
void CommandArgs::param(const std::string& name, bool& p, bool defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_BOOL;
  //将字符串引用的地址传到.data中去，也就是这里存储数据是存储的数据指针
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  //这一步其实是跟ca没啥关系的，只是将p的值赋值为传入的值。
  p = defValue;
  //将ca压入_args数组中。
  _args.push_back(ca);
}
/**
 *
 * 这里继续总结一下class CommandArgs类中的struct CommandArgument结构体
 * 这个结构体是_args数组中的基本元素类型，添加元素的话只能通过调用param()函数，因为只有这个函数中有_args.push_back()操作。
 * 再来观察一下结构体的各个元素: .name .description .type .parsed 这些都好理解，都是参数的一些描述，
 * 来看最核心的.data。这个成员的类型是void*类型的，也就是个指针，这个指针指向哪里呢？指向param()函数中的第二个参数，也就是int& p这个。
 * 这里也就发现眉目了。在_args[]数组中存的各个ca,其实并不是参数本身(至少在ca个各个属性中，没有参数值一说，全是参数的描述)，而是一个具体参数后面的一些描述(name,type...)，
 * 通过.data指向具体的参数，也就是说ca不能单独存在，必须有外部真实的参数变量进行挂靠。
 * 这点从param()中必须要传入一个int& p实打实的int类型引用 和 BundleParams.h中的struct BundleParams结构体中定义的那一堆参数变量可见
 * 终其原因就是_args[]数组中的CommandArgument类型的元素并不会真正的存储参数和参数的值，它只是通过一个.data指针指向一个外部存在的参数。
 * 跟这个参数对应之后，CommandArgument只是用来存储这个参数的一些信息。
 * 至于为什么这么搞，猜测还是为了-help。
 */

void CommandArgs::param(const std::string& name, int& p, int defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_INT;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::param(const std::string& name, float& p, float defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_FLOAT;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::param(const std::string& name, double& p, double defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_DOUBLE;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::param(const std::string& name, std::string& p, const std::string& defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_STRING;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::param(const std::string& name, std::vector<int>& p, const std::vector<int>& defValue, const std::string& desc){
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_VECTOR_INT;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::param(const std::string& name, std::vector<double>& p, const std::vector<double>& defValue, const std::string& desc)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_VECTOR_DOUBLE;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  p = defValue;
  _args.push_back(ca);
}

void CommandArgs::printHelp(std::ostream& os)
{
  if (_banner.size())
    os << _banner << endl;
  os << "Usage: " << _progName << (_args.size()>0?" [options] ":" ");
  // 判断有无必选参数，有的话，循环输出参数名称
  if (_leftOvers.size() > 0) {
    for (size_t i = 0; i < _leftOvers.size(); ++i) {
      if (i > 0)
        os << " ";
      os << _leftOvers[i].name;
    }
  }
  // 判断有无可选参数，有的话，循环输出参数名称
  if (_leftOversOptional.size() > 0) {
    if (_leftOvers.size() > 0)
      os << " ";
    for (size_t i = 0; i < _leftOversOptional.size(); ++i) {
      if (i > 0)
        os << " ";
      os << "[" << _leftOversOptional[i].name << "]";
    }
  }
  // 打印通用消息头
  os << endl << endl;
  os << "General options:" << endl;
  os << "-------------------------------------------" << endl;
  os << "-help / -h           Displays this help." << endl << endl;
  // 判断有无通用参数
  if (_args.size() > 0) {
    os << "Program Options:" << endl;
    os << "-------------------------------------------" << endl;
    // build up option string to print as table
    vector<pair<string, string> > tableStrings;
    // 开辟参数个数大小的空间
    tableStrings.reserve(_args.size());
    size_t maxArgLen = 0;
    for (size_t i = 0; i < _args.size(); ++i) {
      // 判读参数类型是否为bool型
      if (_args[i].type != CAT_BOOL) {
        string defaultValueStr = arg2str(_args[i]);
        // 判断默认值是否为空
        if (! defaultValueStr.empty())
          // 如果默认值不空，则构建下面的pair
          tableStrings.push_back(make_pair(_args[i].name + " " + type2str(_args[i].type), _args[i].description + " (default: " + defaultValueStr + ")"));
        else
          // 如果默认值为空，则构建下面的pair
          tableStrings.push_back(make_pair(_args[i].name + " " + type2str(_args[i].type), _args[i].description));
      } else
        tableStrings.push_back(make_pair(_args[i].name, _args[i].description));
      // 求取每个pair的first和maxArgLen最大参数长度
      maxArgLen = (std::max)(maxArgLen, tableStrings.back().first.size());
    }
    // 按照上面构造的pair对的第一个元素基进行排序
    sort(tableStrings.begin(), tableStrings.end(), CmpPairFirst<string,string>());
    // 在原来的基础上加3个空格
    maxArgLen += 3;
    // 在终端输出构建好的pair
    for (size_t i = 0; i < tableStrings.size(); ++i) {
      os << "-" << tableStrings[i].first;
      for (size_t l = tableStrings[i].first.size(); l < maxArgLen; ++l)
        os << " ";
      os << tableStrings[i].second << endl;
    }
    // TODO should output description for leftOver params?
  }
}

//设置banner，直接赋值。
void CommandArgs::setBanner(const std::string& banner)
{
  _banner = banner;
}

//这个是用于设定其他参数的函数。并根据是否可选，分别存进_leftOversOptional和_leftOvers中
void CommandArgs::paramLeftOver(const std::string& name, std::string& p, const std::string& defValue, const std::string& desc, bool optional)
{
  CommandArgument ca;
  ca.name = name;
  ca.description = desc;
  ca.type = CAT_STRING;
  ca.data = static_cast<void*>(&p);
  ca.parsed = false;
  ca.optional = optional;
  p = defValue;
  //这里根据是否可选分别存进不同的数组。
  if (optional)
    _leftOversOptional.push_back(ca);
  else
    _leftOvers.push_back(ca);
}

//将类型转换成字符串，貌似是用于类型的输出？程序很简单，switch分列就好了。返回字符串。
const char* CommandArgs::type2str(int t) const
{
  switch (t) {
    case CAT_DOUBLE:
      return "<double>";
    case CAT_FLOAT:
      return "<float>";
    case CAT_INT:
      return "<int>";
    case CAT_STRING:
      return "<string>";
    case CAT_BOOL:
      return "<bool>";
    case CAT_VECTOR_INT:
      return "<vector_int>";
    case CAT_VECTOR_DOUBLE:
      return "<vector_double>";
  }
  return "";
}


/**
 * 字符串转参数。string to argument，
 * 其实其功能并不是名字所表示的那样，功能就是将字符串类型的参数值，赋值给.data所指的参数。
 * @param input 输入的字符串
 * @param ca 要赋值的参数。 CommandArgument& 类型的引用。
 */
void CommandArgs::str2arg(const std::string& input, CommandArgument& ca) const
{
  //这里要根据ca的类型进行转化，用switch进行分列。
  switch (ca.type) {
    //以folat为例进行说明
    //流程就是将string转成float然后赋值给.data所指的参数。
    case CAT_FLOAT:
      {
        //定义一个承接float参数的变量aux
        float aux;
        //这个函数在上方定义了。将input字符串转化成float类型的aux，返回是否转化成功。
        bool convertStatus = convertString(input, aux);
        //如果成功，将ca.data指针转换成float*类型，并赋值为aux。
        if (convertStatus) {
          //这里注意一下，.data是的类型这里是void*，使用时要转换成其他基本类型指针。
          //可以发现还是赋值，其他并没有什么操作。
          float* data = static_cast<float*>(ca.data);
          *data = aux;
        }
      }
      break;
    case CAT_DOUBLE:
      {
        double aux;
        bool convertStatus = convertString(input, aux);
        if (convertStatus) {
          double* data = static_cast<double*>(ca.data);
          *data = aux;
        }
      }
      break;
    case CAT_INT:
      {
        int aux;
        bool convertStatus = convertString(input, aux);
        if (convertStatus) {
          int* data = static_cast<int*>(ca.data);
          *data = aux;
        }
      }
      break;
    case CAT_BOOL:
      {
        bool aux;
        bool convertStatus = convertString(input, aux);
        if (convertStatus) {
          bool* data = static_cast<bool*>(ca.data);
          *data = aux;
        }
      }
      break;
    case CAT_STRING:
      {
        string* data = static_cast<string*>(ca.data);
        *data = input;
      }
      break;
    case CAT_VECTOR_INT:
      {
        std::vector<int> aux;
        bool convertStatus = convertString(input, aux);
        if (convertStatus) {
          std::vector<int>* data = static_cast< std::vector<int>* >(ca.data);
          *data = aux;
        }
      }
      break;
    case CAT_VECTOR_DOUBLE:
      {
        std::vector<double> aux;
        bool convertStatus = convertString(input, aux);
        if (convertStatus) {
          std::vector<double>* data = static_cast< std::vector<double>* >(ca.data);
          *data = aux;
        }
      }
      break;
  }
}

/**
 * 将参数值转化成字符串。同样也是用switch进行分列。
 * @param ca 需要转化哪个参数的值
 * @return 返回当然是字符串了。
 */
std::string CommandArgs::arg2str(const CommandArgument& ca) const
{
  
  switch (ca.type) {
    //以float类型参数为例。这里涉及到字符串流sstream的用法，字符串流长用于类型转化，参看：
    //http://blog.csdn.net/xiaogugood/article/details/21447431
    case CAT_FLOAT:
      {
        //将ca.data取出，赋值给data,后面都是操作这个data。
        float* data = static_cast<float*>(ca.data);
        //创建一个字符串流对象
        stringstream auxStream;
        //将data所指的参数值传入到字符串流中，
        // 细聊一句，为什么这里是参数值，因为如果data指向一个int，那么*data就是个int值。所以<<输入就是int值了。
        auxStream << *data;
        //用.str()方法返回字符串流中的字符，同时return。
        return auxStream.str();
      }
    case CAT_DOUBLE:
      {
        double* data = static_cast<double*>(ca.data);
        stringstream auxStream;
        auxStream << *data;
        return auxStream.str();
      }
    case CAT_INT:
      {
        int* data = static_cast<int*>(ca.data);
        stringstream auxStream;
        auxStream << *data;
        return auxStream.str();
      }
    case CAT_BOOL:
      {
        bool* data = static_cast<bool*>(ca.data);
        stringstream auxStream;
        auxStream << *data;
        return auxStream.str();
      }
    case CAT_STRING:
      {
        string* data = static_cast<string*>(ca.data);
        return *data;
      }
    case CAT_VECTOR_INT:
      {
        std::vector<int> * data = static_cast< std::vector<int> * >(ca.data);
        stringstream auxStream;
        auxStream << (*data);
        return auxStream.str();
      }
    case CAT_VECTOR_DOUBLE:
      {
        std::vector<double> * data = static_cast< std::vector<double> * >(ca.data);
        stringstream auxStream;
        auxStream << (*data);
        return auxStream.str();
      }
  }
  return "";
}

//从这个定义中看，是将字符串中的纯字符提取出来，把前后的空格，缩进，换行都给剔除掉。
std::string CommandArgs::trim(const std::string& s) const
{
  //长度为0，说明是空字符串，直接return s
  if(s.length() == 0)
    return s;
  //find_first_not_of 从前往后查找，查找到不是子串中的任意一个字符，返回第一个的位置。这里相当于把前面的空格缩进和换行全部跳过。
  string::size_type b = s.find_first_not_of(" \t\n");

  //find_last_not_of 从后往前查找，查找到不是字串中的任意一个字符，返回最后一个位置。这里相当于把后面的空格缩进换行全都跳过。
  string::size_type e = s.find_last_not_of(" \t\n");

  // 这个判断的意思就是，如果上一句是找s中第一个不是空格缩进换行的字符位置为string::npos，即没有找到，就返回空字符串
  // 没找到说明什么意思呢？上面已经判定了字符串s不是空，但是又找不到不是空格缩进换行的字符，
  // 那就说明s是仅由空格缩进换行构成。所以后面只能返回空字符串""
  if(b == string::npos)
    return "";
  //排除了本身为空和全是空格缩进换行两种情况，剩下的肯定就是比较正常的了。掐头去尾留中间，构造一个string，然后return
  //这里说一下b ,e 这俩都是位置，也就是偏移量，起点都是字符串的首字符。
  //这里用到了string类的构造函数：String(char[] str,int index,int length)
  //在str字符串中的index位置开始，往后取一个length长度的字符串。
  //这样也就明白了第三个参数为什么是e-b+1了。其实是e-(b-1),从e位置，减掉b前一个位置，最后就是从b到e的长度了。
  return std::string(s, b, e - b + 1);
}

/**
 * 这个double类型数组输入，套路跟前面的int类型的一样。唯一不同的是strtod(c,&caux)这个函数的调用。
 * string to double函数，用法原理一样，就是没有了后面的进制参数。所以这里不细说了。
 * @param is
 * @param v
 * @return
 */
std::istream& operator>>(std::istream& is, std::vector<double>& v){
  string s;
  if (! (is >> s) )
    return is;

  const char* c = s.c_str();
  char* caux = const_cast<char*>(c);

  v.clear();
  bool hasNextValue=true;
  while(hasNextValue){
    double i=strtod(c,&caux);
    if (c!=caux){
      c=caux;
      c++;
      v.push_back(i);
    } else
      hasNextValue = false;
  }
  return is;
}

std::ostream& operator<<(std::ostream& os, const std::vector<double>& v)
{
  if (v.size())
    os << v[0];
  for (size_t i=1; i<v.size(); i++)
    os << ";" << v[i];
  return os;
}

//查看参数的parsed属性。看看是否由命令行正确读入了进来。也就是一个参数是否被用户自定了。
bool CommandArgs::parsedParam(const std::string& param) const
{
  std::vector<CommandArgument>::const_iterator it = _args.begin();
  for ( ; it != _args.end(); ++it) {
    //如果找到名称一样的，直接return返回它的parsed属性。
    if (it->name == param) {
      return it->parsed;
    }
  }
  //如果遍历完了都没有，则返回false，说明参数列表中压根没找到这个参数，也是返回false
  return false;
}

