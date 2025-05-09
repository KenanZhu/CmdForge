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
//  VERSION OF LIB    : 1.0.9
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
#define FORGE_VER       std::string("1.0.9")

                                  // Default run sign.
#define DEFT_RSIGN      std::string("> CmdForge [" + FORGE_VER + "] :")

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
extern bool GetDebug(void);       // Get the debyg status.
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

struct ArgFmtData {               // Argument control data type.
    int ArgType=0;                // Argument type.
    string Brief;                 // Brief description.
};

struct OptFmtData {               // Option control data type.
    int OptType=0;                // Option type.
    string Brief;                 // Brief description.
    string LongFmt;               // Long format option.
    string ShortFmt;              // Short format option.
    vector<ArgFmtData> Args;      // Arguments of option.
};

struct CLICfgData {               // CLI configuration data type.
    int MaxStoredCmd=0;           // Maximum stored history commands.

    int InputSleepTime=0;         // Sleep time of command input thread.
    int DetectSleepTime=0;        // Sleep time of key detect thread.

    string ProgramName;           // Name of program.
    string Version;               // Version info of CLI.
};

struct CmdExchangeData {          // Exchanged data type between threads.
    int CursorPos=0;              // Cursor position of command.
    bool ExitFlag=false;          // Exit flag of thread.
    string CurInput;              // Current input.
};

#ifdef __linux__
extern bool _kbhit(void);         // Keyborad hit detect.
extern int _getch(void);          // Get keyborad input.
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
     * These check the callback arguments on parser phase.
     */
    virtual bool PreCheck(vector<string> &OptsArgs);
    virtual bool PostCheck(vector<vector<string>> &OptArgs);

    /*
     * These check the forge valid of this ApiCan on build phase.
     */
    virtual bool BasicCheck(void);
    virtual bool OptValCheck(void);
    virtual bool ArgValCheck(void);
private:
    string s_Brief;               // Brief description of api function.
    vector<string> s_Cmds;        // Commands of api function.
    vector<OptFmtData> s_Opts;    // Options of api command.h
                                  // Arguments arranged by option.
    vector<vector<string>> s_OptArgs;
                                  // Api function pointer callbacked.
    void (*s_API)(vector<vector<string>>);

    void Init(void);

    vector<vector<string>>
         SplitOpts(vector<string> &OptsArgs);
    string
         FormatTextColumns(const string &Str1,const string &Str2,int LeftWidth,int RightWidth);
    void GenHelpInfo(bool isCalled);
public:
    bool Check(void);

    void *API(void);
    void API(vector<string> CmdOptsArgs);

    void SetApi(void (*API)(vector<vector<string>>));
    void SetBrief(const string &Brief);

    bool ExistCmd (const string &Cmd);
    int  CmdIndex (const string &Cmd);
    void AppendCmd(const string &Cmd);

    bool ExistOpt (const string &OptName);
    int  OptIndex (const string &OptName);
    void AppendOpt(OptFmtData Opt);

    vector<string>
        GetAllOpts(void);
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
    int s_ResCmdNum;              // Number of reserved command.
    int s_ResApiNum;              // Number of reserved api can.
    string s_CmdIn;               // Command inputed by user.
    string s_RunSign;             // Runing sign of CmdForge.
    string s_MainCmd;             // Main command.
    vector<int> s_CmdApiTable;    // Command api table.
    vector<string> s_CmdIndex;    // Command index.
                                  // Command index sorted by alphabet.
    vector<string> s_SortedCmdIndex;
    vector<string> s_CmdOptsArgs; // Command splited into command, options, arguments.
    vector<ApiCan> s_ApiCanPool;  // Api can pool.

    vector<string>
         SplitCmd(string &CmdIn);
    void SortStrList(vector<string> &SourceStrList);
private:
    void Init(void);

    static bool CompareStrAlpha(const string &Str1,const string &Str2);
    void SortCmdIndex(void);
public:
    void   SetCmdIn(const string &CmdIn);
    string GetCmdIn(void);
    void   SetMainCmd(const string &MainCmd);
    string GetMainCmd(void);
    void   SetCmdOptsArgs(vector<string> &CmdOptsArgs);
    vector<string>
           GetCmdOptsArgs(void);

    bool ExistCmd (const string &Cmd);
    int  CmdIndex (const string &Cmd);
    void AppendCmd(const string &Cmd);

    bool ExistApiCan (ApiCan ApiCan);
    int  ApiCanIndex (ApiCan ApiCan);
    void AppendApiCan(ApiCan ApiCan);
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
    bool CheckHooks(void);
    void HookApi(const string &ExistCmd,const string &NewCmd);
    void HookApi(const string &Cmd,void (*API)(vector<vector<string>>));
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
    int s_CLIMode;                // Mode of CLI (0:One-Line,1:Interact).
    int s_CurCmdPos;              // Current position of history commands.
    CLICfgData s_Cfg;             // CLI configuration data.
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

    void InputCmdTask(CmdExchangeData *Data);
    void DetecKeyTask(CmdExchangeData *Data);

    void APIhelp   (vector<string> &OptsArgs) override;
    void APIversion(vector<string> &OptsArgs) override;
    void APIsystem (vector<string> &OptsArgs) override;
    void APIexit   (vector<string> &OptsArgs) override;
    void ForkReserved(int Index) override;
private:
    void Init(void);
    bool Check(void);

    void AppendCmdIn (const string &CurCmd);
    void GetLastCmdIn(string &CurCmd);
    void GetNextCmdIn(string &CurCmd);

    void GenHelpInfo(void);
    void GenVersionInfo(void);
public:
    void SetCLICfg(CLICfgData Cfg);
    void SetCLIMode(int Mode);
    void SetCLIVersion(const string &Version);
    void SetCLIMainCmd(const string &MainCmd);

    OptFmtData
         GenOptFmt(const string &LongFmt,const string &ShortFmt,const string &Brief,const vector<ArgFmtData> &Args,int OptType);

    void SetCmdBrief(const string &Cmd,const string &Brief);
    void SetCmdOpt  (const string &Cmd,const OptFmtData &CmdOpt);
    void SetCmdOpts (const string &Cmd,const vector<OptFmtData> &CmdOpts);

    void HookCmdApi(const string &ExistCmd,const string &NewCmd);
    void HookCmdApi(const string &Cmd,void (*API)(vector<vector<string>>));
    void HookCmdApi(const string &Cmd,const string &Brief,const vector<OptFmtData> &Opts,void (*API)(vector<vector<string>>));

    int  MainLoop(const string &RunSign=DEFT_RSIGN);
};