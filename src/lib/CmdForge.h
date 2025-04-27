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
//  VERSION OF LIB    : 1.0.6
// ----------------------------------------------------------------------------

#pragma once

//////////////////// _INCLUDE_
#include <deque>
#include <vector>
#include <sstream>
#include <iostream>
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
#define FORGE_VER       std::string("1.0.6")

                                  // Default run sign.
#define DEFT_RSIGN      std::string("> CmdForge [" + FORGE_VER + "] :")

                                  // Single space.
#define S               std::string(" ")
#define DS              S+S       // Double spaces.
#define TS              S+DS      // Triple spaces.
#define QS              S+TS      // ​​Quadruple spaces.

#define DEFAULT_M       0         // Run Mode: default mode.
#define HIGHPFM_M       1         // Run Mode: high performance mode.
#define BALANCE_M       2         // Run Mode: balanced mode.
#define LOWCOST_M       3         // Run Mode: low cost mode.

#define OPTYPE_O        0x00      // Option type: optional.
#define OPTYPE_M        0x01      // Option type: mandatory.
#define OPTYPE_R        0x02      // Option type: repeatable.
#define OPTYPE_D        0x04      // Option type: default.

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
    void Cout(string Msg,int Endl=1);
    void StdMsg(string Msg,int Level=-1);
    void CurMove(int Offset);
    void Refresh(string RunSign,string CurCmd);
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
    ~ApiCan(void);
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
    vector<string> s_Cmds;        // Command of api function.
    vector<OptFmtData> s_Opts;    // Options of api command.h
                                  // Arguments arranged by option.
    vector<vector<string>> s_OptArgs;
                                  // Api function pointer callbacked.
    void (*s_API)(vector<vector<string>>);

    void Init(void);

    vector<vector<string>> SplitOpts(vector<string> OptsArgs);
    void SortOptArgs(vector<vector<string>> &OptArgs);

    string OutputFormatting(string Str1,string Str2);
    void GenHelpInfo(bool isCalled);
public:
    bool Check(void);

    void *API(void);
    void API(vector<string> CmdOptsArgs);

    void SetApi(void (*API)(vector<vector<string>>));
    void SetBrief(string Brief);

    bool ExistCmd(string Cmd);
    int  CmdIndex(string Cmd);
    void AppendCmd(string Cmd);

    bool ExistOpt(string OptName);
    int  OptIndex(string OptName);
    void AppendOpt(OptFmtData Opt);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE BASE DATA OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class FData:public SysOut
{
public:
    FData(void);
    virtual ~FData(void);
protected:
    int s_ResCmdNum;              // Number of reserved command.
    string s_CmdIn;               // Command inputed by user.
    string s_RunSign;             // Runing sign of CmdForge.
    string s_MainCmd;             // Main command.
    vector<int> s_CmdApiTable;    // Command api table.
    vector<string> s_CmdIndex;    // Command index.
    vector<string> s_CmdOptsArgs; // Command splited by options, arguments.
    vector<ApiCan> s_ApiCanPool;  // Api can pool.

    vector<string> SplitCmd(string CmdIn);
private:
    void Init(void);
public:
    void SetCmdIn(string CmdIn);
    void SetMainCmd(string MainCmd);
    void SetCmdOptArgs(vector<string> CmdOptArgs);

    bool ExistCmd(string Cmd);
    int  CmdIndex(string Cmd);
    void AppendCmd(string Cmd);

    bool ExistApiCan(ApiCan ApiCan);
    int  ApiCanIndex(ApiCan ApiCan);
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

    void HookApi(string Cmd,void (*API)(vector<vector<string>>));
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
    virtual void ForkReserved(int Index);
private:
///
/// NONE FOR THIS CLASS
///
public:
    void CmdParser(string CmdIn);

    void SendOSCmd(string Cmd);

    void ForkApi(string Cmd);
};

////////////////////---------------------------------------

/// THIS CLASS IS THE CONTROLLER OF CMDFORGE
/// 
///////////////////////////////////////////////////////////
class ForgeHwnd:private FParser,private FBuilder
{
public:
    ForgeHwnd(void);
    ~ForgeHwnd(void);
protected:
    CLICfgData s_Cfg;             // CLI configuration data.
    int s_CurCmdPos;              // Current position of history commands.
    deque<string> s_HistoryCmd;   // History command stored.
#ifdef  _WIN32
    DWORD s_Original;             // Original setting of windows terminal.
#elif __linux__
    struct termios s_Original;    // Original setting of linux terminal.
#endif
    void TerminalSet(void);
    void TerminalReset(void);

    void CmdAutoComplete(string *CurCmd);

    void InputCmdTask(CmdExchangeData *Data);
    void DetecKeyTask(CmdExchangeData *Data);
    
    void ForkReserved(int Index) override;
private:
    bool Check(void);

    void StoreCmd(string CurCmd);
    void GetLastCmd(string *CurCmd);
    void GetNextCmd(string *CurCmd);

    void GenHelpInfo(void);
    void GenVersionInfo(void);
public:
    void SetCLICfg(CLICfgData Cfg);
    void SetCLIMode(int Mode);
    void SetCLIVersion(string Version);
    void SetCLIMainCmd(string MainCmd);

    void HookCmdApi(string Cmd,void (*API)(vector<vector<string>>));

    void SetCmdBrief(string Cmd,string Brief);
    void SetCmdOpt(string Cmd,OptFmtData OptFmt);

    int  MainLoop(string RunSign=DEFT_RSIGN);
};