*文档导航*

| [*自述文档*](../README.md) | *API参考手册* | [*架构设计文档*](ArchDesign.md) |

---

## CmdForge-API 参考手册

---

> [!NOTE]
> 这里的方法接口适用的 CmdForge 构建库最低版本为 1.0.9。

### 类和方法

这里提供了详细的说明文档，介绍每个类的具体作用和功能，并对类中的方法进行介绍。

### SysOut 
(*base*)

Brief：维持命令行界面的输出。

### ApiCan 
(*public SysOut*)

Brief：维持，管理命令和**API**包装函数的链接。

### FData 
(*public SysOut*)

Brief：维持，管理**CLI**命令行主界面的数据和包装函数类池（**ApiCanPool**）

### FBuilder
(*virtual public FData*)

Brief：控制，管理构建阶段的命令绑定与命令列表更新。

### FParser 
(*virtual public FData*)

Brief：处理，解析命令输入，并传递经过标准处理后的命令输入。

### ForgeHwnd
(*private FBuilder, private FParser*)
 
Brief：主界面构建与运行维持类。

|**类方法**|**方法简介**|
|:--|:--|
|||
|**构造与析构**||
|*ForgeHwnd*(*void*)|默认构造函数，该构造方式只支持交互模式|
|*ForgeHwnd*(*int*, **argv[]*)|标准构造函数|
|*~ForgeHwnd*(*void*)|默认析构函数|
|||
|**protected :**||
|***CLICfgData***  *s_Cfg*|命令行设置数据|
|***int***  *s_CurCmdPos*|当前命令在命令记录列表中的位置|
|***std::vector< std::string >***  *s_HistoryCmd*|命令记录列表（历史命令）|
|***struct termios***  *s_Original*|（__ linux __）命令行终端原始模式配置|
|***DWORD*** *s_Original*|（_WIN32）命令行终端原始模式配置|
|***void***  *TerminalSet*(*void*)|设置命令行终端模式|
|***void***  *TerminalReset*(*void*)|恢复命令行终端初始模式|
|***void***  *AutoCompleteCmd*(*std::string* &)|命令自动补全|
|***void***  *InputCmdTask*(*ThreadsSharedData* &)|命令输入任务|
|***void***  *DetectKeyTask*(*ThreadsSharedData* &)|按键检测/回显任务|
|***void***  *APIhelp*(*std::vector< std::string >* &) *override*|预留命令函数：帮助信息|
|***void***  *APIversion*(*std::vector< std::string >* &) *override*|预留命令函数：版本信息|
|***void***  *APIsystem*(*std::vector< std::string >* &) *override*|预留命令函数：系统接口|
|***void***  *APIexit*(*std::vector< std::string >* &) *override*|预留命令函数：退出进程|
|***void***  *ForkReseved*(*int*) *override*|回调预留命令函数|
|||
|**private :**||
|***void***  *Init*(*void*)|系统构建初始化|
|***bool***  *Check*(*void*)|系统构建自检|
|***void***  *AppendCmdIn*(*const std::string* &)|存储当前命令|
|***void***  *GetLastCmdIn*(*std::string* &)|（如果有）获取上个记录的命令|
|***void***  *GetNextCmdIn*(*std::string* &)|（如果有）获取下个记录的命令|
|***void***  *GenHelpInfo*(*void*)|生成帮助信息|
|***void***  *GenVersionInfo*(*void*)|生成版本信息|
|||
|**public :**||
|***void***  *SetCLICfg*(*CLICfgData*)|设置命令行运行参数|
|***void***  *SetCLIMode*(*int*)|设置命令行运行模式|
|***void***  *SetCLIVersion*(*const std::string* &)|设置命令行程序版本|
|***void***  *SetCLIMainCmd*(*const std::string* &)|设置主命令|
|***OptFmtData***|||
|*GenOptFmt*(*const std::string* &, *const std::string* &, *const std::string* &, *const std::vector< ArgFmtData >* &, *int*)|生成选项格式数据|
|***void***  *SetCmdBrief*(*const std::string* &, *const std::string* &)|设置命令的简介说明|
|***void***  *SetCmdOpt*(*const std::string* &, *const OptFmtData* &)|设置命令选项格式|
|***void***  *SetCmdOpts*(*const std::string* &, *const std::vector< OptFmtData >* &)|设置命令选项格式|
|***void***  *HookCmdApi*(*const std::string* &, *const std::string*)|链接新命令至已有命令|
|***void***  *HookCmdApi*(*const std::string* &, *void* ( * ) (*std::vector< std::vector< std::string > >*))|链接命令与包装函数|
|***void***  *HookCmdApi*(*const std::string* &, *const std::string* &, *const OptFmtData* &,*void* ( * ) (*std::vector< std::vector< std::string > >*))|链接命令与包装函数|
|***int***   *MainLoop*(*const std::string* &=DEFT_RSIGN)|启动界面主循环|