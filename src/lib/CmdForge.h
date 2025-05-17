//  \FILE  : CmdForge.h
//  \BRIEF : CmdForge class,functions&data types prototypes.
//
//  \AUTHOR: KenanZhu (NANOKI)
// ----------------------------------------------------------------------------
//  CmdForge <https://github.com/KenanZhu/CmdForge>
//  Copyright (C) 2025  KenanZhu (NANOKI)
// 
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
// 
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
//  DATE OF FIRST EDIT: 2025-02-26
//  VERSION OF LIB    : 1.1.1
// ----------------------------------------------------------------------------

#pragma once

//////////////////// _INCLUDE_
#include <deque>
#include <vector>
#include <cctype>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_set>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif

//////////////////// _DEFINE_

                                  // Version of 'CmdForge'.
#define FORGE_VER       std::string("1.1.1")

                                  // Default run sign.
#define DEFT_RSIGN      std::string("> CmdForge [" + FORGE_VER + "] : ")

                                  // Single space.
#define S               std::string(" ")
#define DS              S+S       // Double spaces.
#define TS              S+DS      // Triple spaces.
#define QS              S+TS      // ​​Quadruple spaces.

#define ONELINE_M       0         // CLI Mode: one-line mode.
#define INTRACT_M       1         // CLI Mode: interact mode.

#define DEFAULT_M       0         // Run Mode: default mode.
#define HIGHPFM_M       1         // Run Mode: high performance mode.
#define BALANCE_M       2         // Run Mode: balanced mode.
#define LOWCOST_M       3         // Run Mode: low cost mode.

#define OPTYPE_O        0x00      // Option type: optional.
#define OPTYPE_M        0x01      // Option type: mandatory.
#define OPTYPE_R        0x02      // Option type: repeatable.
#define OPTYPE_D        0x04      // Option type: default.

//////////////////// _CONSTANT_

#ifdef _DEBUG
static bool g_IsDebug=true;       // Only debug mode will show the message sign/log.
extern void SetDebug(bool);       // Enable/Disable debug.
extern bool GetDebug(void);       // Get the debug status.
#endif // _DEBUG

////////////////////---------------------------------------

using namespace std; /// STANDARD NAMESPACE

////////////////////---------------------------------------

class SysOut;        /// FORWARD DECLARATION
class ApiCan;
class FData;
class FBuilder;
class FParser;
class ForgeHwnd;

struct ArgumentData {             // Argument control data type.
    int ArgType=0;                // Argument type.
    string Brief;                 // Brief description.
};

struct OptionData {               // Option control data type.
    int OptType=0;                // Option type.
    string Brief;                 // Brief description.
    string LongFmt;               // Long format option.
    string ShortFmt;              // Short format option.
    vector<ArgumentData> Args;    // Arguments of option.
};

struct CLIConfigData {            // CLI configuration data type.
    int MaxStoredCmd=0;           // Maximum stored history commands.
    int InputSleepTime=0;         // Sleep time of command input thread.
    int DetectSleepTime=0;        // Sleep time of key detect thread.
    string ProgramName;           // Name of program.
    string Version;               // Version info of CLI.
};

struct RunTimeCmdData {           // Runtime command data.
    int CursorPos=0;              // Cursor position of command.
    bool ExitFlag=false;          // Exit flag of thread.
    string CurInput;              // Current input.
};

#ifdef __linux__
extern bool _kbhit(void);         // Keyboard hit detect.
extern int _getch(void);          // Get keyboard input.
#endif

////////////////////---------------------------------------

/// THIS CLASS IS THE SYSTEM INFORMATION OUTPUT
///
///////////////////////////////////////////////////////////
class SysOut
{
public:
    SysOut(void)=default;
    virtual ~SysOut(void)=default;
protected:
///
/// NONE FOR THIS CLASS
///
private:
    int s_CurInputLength=0;       // Current input length in terminal.
public:
    void Cout(const string &Msg,int Endl=1);
    void StdMsg(const string &Msg,int Level=-1);
    void CurMove(int Offset);
    void Refresh(string &RunSign,string CurCmd);
    void SetInputLength(int CurInputLength);
    int  GetInputLength(void);
};
////////////////////---------------------------------------

/// THIS CLASS IS THE CAN OF API FUNCTION
///
///////////////////////////////////////////////////////////
class ApiCan:public SysOut
{
public:
    ApiCan(void);
    virtual ~ApiCan(void) override;
protected:
    /*
     * These check on parser phase.
     */
    virtual bool ParserCheckPre (vector<string> &OptsArgs);
    virtual bool ParserCheckPost(vector<vector<string>> &OptArgs);

    /*
     * These check on build phase.
     */
    virtual bool BuildCheckBasic(void);
    virtual bool BuildCheckCmds (void);
    virtual bool BuildCheckOpts (void);
    virtual bool BuildCheckArgs (void);
private:
    string s_Brief;               // Brief description of api function.
    vector<string> s_Cmds;        // Commands of api function.
    vector<OptionData> s_Opts;    // Options of api command.h
                                  // Arguments arranged by option.
    vector<vector<string>> s_OptArgs;
                                  // Api callback function pointer.
    void (*s_API)(vector<vector<string>>);

    void Init(void);

    vector<vector<string>> SplitCmdByOpts(vector<string> &OptsArgs);
    string FormatTextColumns(const string &Str1,const string &Str2,int LeftWidth,int RightWidth);
    void GenHelpInfo(bool isCalled);
public:
    bool BuildCheck(void);

    void *API(void);
    void API(vector<string> CmdOptsArgs);

    void SetApi(void (*API)(vector<vector<string>>));
    void SetBrief(const string &Brief);

    int  CmdsCount (void);
    bool ExistCmd  (const string &Cmd);
    int  IndexOfCmd(const string &Cmd);
    void AppendCmd (const string &Cmd);
    string CmdAt(int Index);

    int  OptsCount (void);
    bool ExistOpt  (const string &OptName);
    int  IndexOfOpt(const string &OptName);
    void AppendOpt (OptionData Opt);
    OptionData OptAt(int Index);

    vector<string> GetAllOpts(void);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE BASE DATA OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class FData:public SysOut
{
public:
    FData(void);
    virtual ~FData(void) override;
protected:
///
/// NONE FOR THIS CLASS
///
private:
    int s_ResCmdNum;              // Number of reserved command.
    int s_ResApiNum;              // Number of reserved api can.
    string s_CmdIn;               // Command inputted by user.
    string s_MainCmd;             // Main command.
    vector<int> s_CmdToApiMap;    // Command api table.
    vector<string> s_CmdIndex;    // Command index.
                                  // Command index sorted by alphabet.
    vector<string> s_SortedCmdIndex;
    vector<string> s_CmdOptsArgs; // Command split into command, options, arguments.
    vector<ApiCan> s_ApiCanPool;  // Api can pool.

    void Init(void);
    static bool CompareStrAlpha(const string &Str1,const string &Str2);

    vector<string> SplitCmdBySpace(string &CmdIn); 
    void SortCmdIndex(void);
public:
    static vector<string> SplitStrBySpace(string &Str);
    static void SortStrListByAlpha(vector<string> &StrList);

    void   SetResCmdNum(int ResCmdNum);
    int    GetResCmdNum(void);
    void   SetResApiNum(int ResApiNum);
    int    GetResApiNum(void);

    void   SetCmdIn(const string &CmdIn);
    string GetCmdIn(void);
    void   SetMainCmd(const string &MainCmd);
    string GetMainCmd(void);
    void   AddMapping(int CmdIndex,int ApiIndex);
    int    GetMapping(int CmdIndex);
    vector<string> GetSortedCmdIndex(void);
    void   SetCmdOptsArgs(vector<string> &CmdOptsArgs);
    vector<string> GetCmdOptsArgs(void);

    int  CmdsCount (void);
    bool ExistCmd  (const string &Cmd);
    int  IndexOfCmd(const string &Cmd);
    void AppendCmd (const string &Cmd);
    string CmdAt(int Index);

    int  ApiCanCount  (void);
    bool ExistApiCan  (ApiCan ApiCan);
    int  IndexOfApiCan(ApiCan ApiCan);
    void AppendApiCan (ApiCan ApiCan);
    ApiCan *ApiCanAt(int Index);
    ApiCan *ApiCanAt(const string &Cmd);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE BUILDER OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class FBuilder:virtual public FData
{
public:
    FBuilder()=default;
    virtual ~FBuilder()=default;
protected:
///
/// NONE FOR THIS CLASS
///
private:
///
/// NONE FOR THIS CLASS
///
public:
    bool BuildCheckHooks(void);
    void HookApi(const string &ExistCmd,const string &NewCmd);
    void HookApi(const string &Cmd,void (*API)(vector<vector<string>>));
    void HookBrief(const string &Cmd,const string &Brief);
    void HookOpt  (const string &Cmd,const OptionData &Opt);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE PARSER OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class FParser:virtual public FData
{
public:
    FParser()=default;
    virtual ~FParser()=default;
protected:
    virtual void APIhelp   (vector<string> &OptsArgs);
    virtual void APIversion(vector<string> &OptsArgs);
    virtual void APIsystem (vector<string> &OptsArgs);
    virtual void APIexit   (vector<string> &OptsArgs);
    virtual void ForkReserved(int Index);
private:
///
/// NONE FOR THIS CLASS
///
public:
    void ParserCmd(void);
    void ForkApi(const string &Cmd);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE CONTROLLER OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class ForgeHwnd:private FParser,private FBuilder
{
public:
    ForgeHwnd(void);
    ForgeHwnd(int argc,char *argv[]);
    virtual ~ForgeHwnd(void) override;
protected:
    string s_RunSign;             // Runing sign of CmdForge.
    int s_CLIMode;                // Mode of CLI (0:One-Line,1:Interact).
    int s_CurCmdPos;              // Current position of history commands.
    CLIConfigData s_Cfg;          // CLI configuration data.
    deque<string> s_HistoryCmd;   // History command stored.
#ifdef  _WIN32
    DWORD s_Original;             // Original setting of windows terminal.
#elif __linux__
    struct termios s_Original;    // Original setting of linux terminal.
#endif
    void SetTerminal(void);
    void ResetTerminal(void);

    void Complete(string &SourceStr,vector<string> &TargetStrList);
    void CompleteOpt(string &CurCmd,string &CurOpt);
    void CompleteCmd(string &CurCmd);
    void AutoCompleteCmd(string &CurCmd);

    void InputCmdTask(RunTimeCmdData *Data);
    void DetecKeyTask(RunTimeCmdData *Data);

    void APIhelp   (vector<string> &OptsArgs) override;
    void APIversion(vector<string> &OptsArgs) override;
    void APIsystem (vector<string> &OptsArgs) override;
    void APIexit   (vector<string> &OptsArgs) override;
    void ForkReserved(int Index) override;
private:
    void Init(void);
    bool BuildCheck(void);

    void AppendCmdIn (const string &CurCmd);
    void GetLastCmdIn(string &CurCmd);
    void GetNextCmdIn(string &CurCmd);

    void GenHelpInfo(void);
    void GenVersionInfo(void);
public:
    void SetCLICfg(CLIConfigData Cfg);
    void SetCLIMode(int Mode);
    void SetCLIVersion(const string &Version);
    void SetCLIMainCmd(const string &MainCmd);

    OptionData GenOptFmt(const string &LongFmt,const string &ShortFmt,const string &Brief,const vector<ArgumentData> &Args,int OptType);

    void SetCmdBrief(const string &Cmd,const string &Brief);
    void SetCmdOpt  (const string &Cmd,const OptionData &CmdOpt);
    void SetCmdOpts (const string &Cmd,const vector<OptionData> &CmdOpts);

    void HookCmdApi(const string &ExistCmd,const string &NewCmd);
    void HookCmdApi(const string &Cmd,void (*API)(vector<vector<string>>));
    void HookCmdApi(const string &Cmd,const string &Brief,const vector<OptionData> &Opts,void (*API)(vector<vector<string>>));

    int  MainLoop(const string &RunSign=DEFT_RSIGN);
};