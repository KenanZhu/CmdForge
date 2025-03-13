### 基本原理
![原理结构](./res/images/Principle-Arch.png#pic_center)

如图所示，**CmdForge**的命令行界面维持类**ForgeHwnd**主要由**FBuilder**类和**FParser**类继承而来（*关于类及其方法的详细说明请见文档：[API参考手册](APIGuide.md)*）。这里的**FBuiler**和**FParser**虚继承自**FData**。通过虚继承机制，**ForgeHwnd**作为最终派生类将持有唯一的**FData**实例，**FData**主要负责维护回调函数类**ApiCan**池**ApiCanPool**。整体继承结构呈现菱形虚继承架构。

![构建阶段](./res/images/Principle-BuildPhase.png#pic_center)

如图所示，构建阶段，通过**ForgeHwnd**类方法进行调用式构造，并通过**FBuilder**类处理构造参数最终访问并修改**FData**维护的**ApiCanPool**，记录并管理构造阶段产生的数据和元数据。

![解析阶段](./res/images/Principle-ParserPhase.png#pic_center)

用户输入命令，经过**ForgeHwnd**捕获并通过调用**FData**的数据处理方法将原始输入转换为标准格式后经由**FParser**处理后使用内部方法访问并调用**FData**中**ApiCanPool**中的回调函数，由此完成命令的解析和调用。

## 实例构建详细说明

正如上文提到，**CmdForge**的构建方法是基于类方法的。也就是说，你只需要实例化一个命令行界面维持类即可完成交互界面的搭建。不过，除此之外，还有一些其它的准备工作。

这里我们引入**包装函数**的概念。该包装函数需要你自己创建一个包装函数头文件。在demo实例中创建了头文件***API.h***。其中包装函数主要参数和构造类型如下所示：

```cpp
// 包装函数模板

void PackageFunc(std::vector<std::vector<string>>)
{
	// 这里添加你自己的参数处理逻辑

	// 当然，可以在这里调用你自己的API，需要注意的是，该包装函数只起到回调
	// 的作用，并不负责相应参数的类型的判断和转换的工作。
	return;
}
```

该函数接受一个二维的 *std::string* 向量容器参数，里面包含了经由**FParser**分析处理后的命令选项和参数数据。返回类型为 *void*。

在**API.h**的具体实例中包含了两个包装函数如下：

```cpp
//  \FILE  : APIs.h
//  \BRIEF : 在这里创建你的API包装函数。

#pragma once

#include <vector>
#include <iostream>

using namespace std;

////    包装函数必须是如下的形式:
////
////    ' void (*) (vector<vector<string>>) '
////
////    其它类型无法进行使用.

void process_1(vector<vector<string>> OptArgs)
{
////    详细的代码逻辑实现如下
///////////////////////////////////////////////////////////
	cout<<"callback process_1 by command: '"+OptArgs[0][0]+"'."<<endl;

///////////////////////////////////////////////////////////
////    在这里判断和转换 '[-opts]' 和 '[-args]'参数 
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
////   在这里通过转换后的'[-opts]' 和 '[-args]'数据来调用
////   你的函数API或者直接在这里编写逻辑
///////////////////////////////////////////////////////////
	return;
}
void process_2(vector<vector<string>> OptArgs)
{
////    和上述同理
	cout<<"callback process_2 by command: '"+OptArgs[0][0]+"'."<<endl;
///////////////////////////////////////////////////////////
	return;
}
```

这里生明并定义了两个包装函数：*process_1 和 *process_2* ,具体说明参见代码中注释。

接下来是主体界面的构建方式，在主函数中我们首先需要包含**CmdForge**库的头文件**CmdForge.h**和包装函数的头文件(在demo实例中我们指**API.h**）。首先需要创建一个**ForgeHwnd**类的实例，根据需要可以创建CLI配置数据结构体；命令选项格式数据；命令选项参数格式数据。接下来同样以demo实例中的**Demo.cpp**文件进行详细的说明和介绍。

下面是**Demo.cpp**文件的详细内容：

```cpp
//  \FILE  : Demo.cpp
//  \BRIEF : A simple usage example of 'CmdForge'

#include "APIs.h"                 // 你的API包装函数。
#include "./lib/CmdForge.h"       // 'CmdForge' 构建库头文件。

int main() {

    // 初始化你的 'CMD FORGE'
    ///////////////////////////////////////////////////////
    ForgeHwnd CLIF;               // CLI 主界面维持类。
    ArgFmtData ArgFmt;            // 选项参数格式数据。
    CLICfgData CLICfg;            // CLI 配置数据。.
    OptFmtData OptFmt;            // 选项格式数据。
    
    // 设置CmdForge的运行参数
    ///////////////////////////////////////////////////////
    CLICfg.InputSleTime=100;
    CLICfg.DetectSleTime=100;
    CLICfg.MaxStoredCmd=20;
    CLICfg.VerMode=VER_M_ALPA;
    CLICfg.Version="1.0.0.0";
    CLIF.SetCLICfg(CLICfg);
    
    // 设置你的主命令
    ///////////////////////////////////////////////////////
    CLIF.SetCLIMainCmd("test");
    
    // 将命令与包装函数进行连接
    ///////////////////////////////////////////////////////
    CLIF.HookCmdApi("-doit",process_1);
    CLIF.HookCmdApi("-set",process_2);
    
    // 添加选项 "-a" 到命令 "-doit":
    CLIF.SetCmdBrief("-doit","do some thing");
    OptFmt.Optional=true;
    OptFmt.LongFmt="-bya";
    OptFmt.ShortFmt="-a";
    OptFmt.Brief="do some thing by method a";
    OptFmt.Args.push_back(ArgFmt);
    OptFmt.Args.push_back(ArgFmt);
    CLIF.SetCmdOpt("-doit",OptFmt);
    
    // [Tips] 你可以在一个命令中添加不同的选项:
    OptFmt.Optional=false;
    OptFmt.LongFmt="-byb";
    OptFmt.ShortFmt="-b";
    OptFmt.Brief="do some thing by method b";
    CLIF.SetCmdOpt("-doit",OptFmt);
    
    OptFmt.Optional=false;
    OptFmt.LongFmt="-byc";
    OptFmt.ShortFmt="-c";
    OptFmt.Brief="do some thing by method c";
    CLIF.SetCmdOpt("-doit",OptFmt);

    // 添加选项 "-a" 到命令 "-set":
    CLIF.SetCmdBrief("-set","set some thing");
    OptFmt.Optional=true;
    OptFmt.LongFmt="-bya";
    OptFmt.ShortFmt="-a";
    OptFmt.Brief="set some thing by method a";
    CLIF.SetCmdOpt("-set",OptFmt);
    
    // [Warn] 在一个命令中添加相同的选项是不被允许的:
    CLIF.SetCmdOpt("-set",OptFmt);

    // [Tips] 你可以将不同的命令连接到同一个包装函数:
    CLIF.HookCmdApi("-do",process_1);
    
    // [Warn] 将相同的命令连接到不同的的包装函数是不被允许的:
    CLIF.HookCmdApi("-do",process_2);
    
    // 启动主循环
    ///////////////////////////////////////////////////////
    CLIF.MainLoop();
    
    return 0;
}
```

在上述文件源程序中，我们设置了**CLI**的主命令：“test”，同时借助**ForgeHwnd**的**HookCmdApi**方法添加了两个自定义子命令：“-doit”，“-set”。通过自定义选项格式数据，并将自定义好的数据借助**ForgeHwnd**的**SetCmdOpt**方法分别为子命令 “-doit” 添加了三个自定义选项 “-bya/-a”，“-byb/-b”，“-byc/-c”；为子命令 “-set“ 添加了一个自定义选项 “-bya/-a”。完成了命令行界面的基础构建。同时，同时我们在示例demo中给出了两个方法提示和两个方法警告：

【提示】
1. 你可以在同一个子命令中添加不同的选项。
2. 你可以为同一个包装函数链接到不同的子命令。
【警告】
4. 不要在同一个子命令中添加相同命名的选项。
5. 不要将相同的子命令链接到不同的包装函数中。

### 自行构建说明

该部分请参照demo实例中给出的使用方法，结合附录1内容的类和方法，进行构建。