*文档导航*

| [*自述文档*](../README.md) | *API参考手册* | [*架构设计文档*](ArchDesign.md) |

---

## CmdForge-API 参考手册

---

### 类和方法

这里提供了详细的说明文档，介绍每个类的具体作用和功能，并对类中的方法进行介绍。

### SysOut 
(base)

Brief：维持命令行界面的输出。
### ApiCan 
(public SysOut)

Brief：维持，管理命令和**API**包装函数的链接。
### FData 
(public SysOut)

Brief：维持，管理**CLI**命令行主界面的数据和包装函数类池（**ApiCanPool**）
### FBuilder
(virtual public FData)

Brief：控制，管理构建阶段的命令绑定与命令列表更新。
### FParser 
(virtual public FData)

Brief：处理，解析命令输入，并传递经过标准处理后的命令输入。
### ForgeHwnd
 (public FBuilder, public FParser)
 
Brief：主界面构建与运行维持类。

|**protected:**||
|:--|:--|
|||
|***CLICfgData***  *s_Cfg*|命令行设置数据|
|***int***  *s_CurCmdPos*|当前命令在命令记录列表中的位置|
|***std::vector< std::string >***  *s_HistoryCmd*|命令记录列表（历史命令）|
|***struct termios***  *s_Original*|（__ linux __）命令行终端原始模式配置|
|***DWORD***  *s_Original*|（_WIN32）命令行终端原始模式配置|
|***void***  *CmdAutoComplete*(*std::string**)|命令自动补全|
|***void***  *InputCmdTask*(*ThreadsSharedData**)|命令输入任务|
|***void***  *DetectKeyTask*(*ThreadsSharedData**)|按键检测/回显任务|
|**private:**||
|***bool***  *Check*(*void*)|系统构建自检|
|***void***    *StoreCmd*(*std::string*)|存储当前命令|
|***void***   *GetLastCmd*(*std::string* *)|（如果有）获取上个记录的命令|
|***void***   *GetNextCmd*(*std::string* *)|（如果有）获取下个记录的命令|
|***void***   *TerminalSet*(*void*)|设置命令行终端模式|
|***void***   *TerminalReset*(*void*)|恢复命令行终端初始模式|
|***void***   *GenHelpInfo*(*void*)|生成帮助信息|
|***void***   *GenVersionInfo*(*void*)|生成版本信息|
|***void***   *ForkReseved*(*int*)|回调预留命令函数|
|**public:**||
|*ForgeHwnd()*|构造函数|
|***void***  *SetCLICfg*(*CLICfgData*)|设置命令行运行参数|
|***void***  *SetCLIMode*(*int*)|设置命令行运行模式|
|***void***  *SetCLIVersion*(*std::string*)|设置命令行程序版本|
|***void***  *SetCLIMainCmd*(*std::string*)|设置主命令|
|***void***  *HookCmdApi(*std::string*, *void* ( *) (*std::vector< std::vector< std::string > >*))|链接命令与包装函数|
|***void***  *SetCmdBrief*(*std::string, std::string*)|设置命令的简介说明|
|***void***  *SetCmdOpt*(*std::string*, *OptFmtData*)|设置命令选项格式|
|***int***  *MainLoop*(*std::string*=*std::string*("> CmdForge :"))|启动界面主循环|