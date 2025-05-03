//  \FILE  : CmdForge.cpp
//  \BRIEF : CmdForge classes,functions&data types implementations.
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
//  VERSION OF LIB    : 1.0.7
// ----------------------------------------------------------------------------

#include "CmdForge.h"

#ifdef __linux__
bool _kbhit(void)
{
    struct timeval tv={0, 0};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO,&fds);
    return select(STDIN_FILENO+1,&fds,nullptr,nullptr,&tv)>0;
}

int _getch(void)
{
    char c=0;

    if (read(STDIN_FILENO,&c,1)<0) {
        return 0;
    }
    return c;
}
#endif

/////////////////////////////////////////////////////////////////////
///    CLASS : SysOut
/////////////////////////////////////////////////////////////////////

///
///    USE DEFAULT CONSTRUCTOR
///

///    PROTECTED :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PRIVATE :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////
void SysOut::Cout(string Msg,int Endl)
{
    if (Endl== 0) cout<<Msg<<flush;
    if (Endl== 1) cout<<Msg<<endl<<flush;

    return;
}

void SysOut::StdMsg(string Msg,int Level)
{
    string LevelSign;

    switch (Level) {
    case 0:LevelSign=DS; break;
    case 1:LevelSign="[info ] "; break;
    case 2:LevelSign="[warn ] "; break;
    case 3:LevelSign="[error] "; break;
    case 4:LevelSign="[fatal] "; break;
    default: this->Cout(Msg); return;
    }
    this->Cout(LevelSign+Msg);

    return;
}

void SysOut::CurMove(int SetPos)
{
    int NewPos;

    NewPos=max(0,min(SetPos,(int)s_CurInputLength));

    this->Cout("\033[s", 0);
    this->Cout("\033["+to_string(NewPos+1)+"G", 0);

    return;
}

void SysOut::Refresh(string RunSign,string CurCmd)
{
    this->SetInputLength((int)(RunSign.size()+CurCmd.size()));

    this->Cout("\33[2K\r"+RunSign+CurCmd, 0);

    return;
}

void SysOut::SetInputLength(int CurInputLength)
{
    s_CurInputLength=CurInputLength;

    return;
}

int SysOut::GetInputLength(void)
{
    return s_CurInputLength;
}

/////////////////////////////////////////////////////////////////////
///    CLASS : ApiCan
/////////////////////////////////////////////////////////////////////
ApiCan::ApiCan(void)
{
    this->Init();
}
ApiCan::~ApiCan(void)
{
    this->Init();
}
///    PROTECTED :
/////////////////////////////////////////////////
bool ApiCan::PreCheck(vector<string> &OptsArgs)
{
    int i,Opts;
    bool State;
    
    State=true;
    Opts=(int)s_Opts.size();
    if (OptsArgs.size()==0) {               // CHECK: no [-opt] part.
        if (!Opts) return State=true;

        this->StdMsg("no [-opt] part",0);
        this->StdMsg("need help? use [-opt] '-help/-h'",0);
        State=false;
    }                                       // CHECK: '-help/-h' [-opt].
    else if (OptsArgs[0]=="-help"||OptsArgs[0]=="-h") {
        this->GenHelpInfo(true);
        State=false;
    }                                       // CHECK: use default [-opt].
    else if (!this->ExistOpt(OptsArgs[0])) {
        for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_D) {
            OptsArgs.insert(OptsArgs.begin(),s_Opts[0].LongFmt);
        }
        State=true;
    }
    return State;
}

bool ApiCan::PostCheck(vector<vector<string>> &OptArgs)
{
    bool State;
    int i,Index,ExpArgs,ActArgs,ExpOpts,ActOpts,RepeatTimes,Size,Opts;
    unordered_multiset<string> InputOpts;

    State=true;
    ExpOpts=ActOpts=0;
    Size=(int)OptArgs.size(),Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_M) ExpOpts++;
    for (i=0;i<Size;i++) InputOpts.insert(OptArgs[i][0]);

    // Check the options.
    //
    // It will check if lost mandatory options.
    // It will check if the unrepeatable option is repeat.
    if (Opts&&Size==0) {this->StdMsg("no valid [-opt].",0); return State=false;}

    // Find if lost mandatory options.
    for (i=0;i<Opts;i++) {
        if (!(s_Opts[i].OptType&OPTYPE_M)) continue;

        if (!InputOpts.count(s_Opts[i].LongFmt)&&
            !InputOpts.count(s_Opts[i].ShortFmt)) {
            this->StdMsg("mandatory [-opt] '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"' is not found",0);
            continue;
        }
        ActOpts++;
    }
    if (ActOpts<ExpOpts) {
        this->StdMsg("lost "+to_string(ExpOpts-ActOpts)+" mandatory [-opt](s)",0);
        return State=false;
    }
    // Find if the unrepeatable option is repeat.
    for (i=0;i<Opts;i++) {
        if (s_Opts[i].OptType&OPTYPE_R) continue;

        RepeatTimes=(int)(InputOpts.count(s_Opts[i].LongFmt)+InputOpts.count(s_Opts[i].ShortFmt));
        if (RepeatTimes>1) {
            this->StdMsg("got "+to_string(RepeatTimes)+" [-opt](s) '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"' in command, but it is unrepeatable",0);
            continue;
        }
    }

    // Check the arguments.
    // :
    // The OptArgs is splited by exist option, so no need to check
    // option.
    for (i=0;i<Size;i++) {
        Index=this->OptIndex(OptArgs[i][0]);

        ExpArgs=(int)s_Opts[Index].Args.size();
        ActArgs=(int)OptArgs[i].size()-1;
        if (ExpArgs!=ActArgs) {
            this->StdMsg("[-opt] '"+OptArgs[i][0]+"' expects "+to_string(ExpArgs)+" [-arg](s), but got "+to_string(ActArgs),0);
            State=false;
        }
    }
    return State;
}

bool ApiCan::BasicCheck(void)
{
    bool State;
    int Opts,Cmds;
    
    State = true;
    Opts=(int)s_Opts.size();
    Cmds=(int)s_Cmds.size();

    if (s_API==nullptr) {
        this->StdMsg("no api was hooked in a api can",3);
        return State=false;
    }
    else if (s_Cmds.size()==0) {
        this->StdMsg("no [-cmd] was hooked with a api",3);
        return State=false;
    }
    else if (s_Opts.size()==0) {
        this->StdMsg("no [-opt] include in [-cmd] '"+s_Cmds[0]+"'",2);
        return State=true;
    }
    return State;
}

bool ApiCan::OptValCheck(void)
{
    bool State;
    int i,Opts,ExpDefault,ActDefault;
    
    State=true;
    ExpDefault=1;
    ActDefault=0;
    Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) {
        if (s_Opts[i].LongFmt.empty()&&s_Opts[i].ShortFmt.empty()) {
            this->StdMsg("an empty name of a option was added in [-cmd] '"+s_Cmds[0]+"'",2);
        }
    }
    for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_D) ActDefault++;

    if (ActDefault>ExpDefault) {
        this->StdMsg("[-cmd] '"+s_Cmds[0]+"' expect only 1 default [-opt], but you set "+to_string(ActDefault)+" default [-opt]",3);
        return State=false;
    }
    return State;
}

bool ApiCan::ArgValCheck(void)
{
    bool State;
    int i,Opts;

    State=true;
    Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) {
        if (s_Opts[i].Args.size()==0) {
            this->StdMsg("you should have least 1 [-arg] in [-opt] '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"'",2);
        }
    }
    return State;
}
///    PRIVATE :
/////////////////////////////////////////////////
void ApiCan::Init(void)
{
    s_API=nullptr;
    s_Cmds.clear();
    s_Brief.clear();
    s_OptArgs.clear();

    return;
}

vector<vector<string>> ApiCan::SplitOpts(vector<string> OptsArgs)
{
    vector<string> TempOptArg;
    vector<vector<string>> TempOptArgs;
    int i,j,k,Size;
        
    Size=(int)OptsArgs.size();
    for (i=0;i<Size;i++) {
        if (this->ExistOpt(OptsArgs[i])) {
            TempOptArg.push_back(OptsArgs[i]);
            i++;
        }
        // Pass the unknow input.
        for (j=i;j<Size&&!TempOptArg.empty();j++) {
            if (!this->ExistOpt(OptsArgs[j])&&j+1!=Size) continue;
            if (this->ExistOpt(OptsArgs[j])) j-=1;
            else if (j+1==Size) j+=0;

            // Store the argumets.
            for (k=i;k<=j;k++) TempOptArg.push_back(OptsArgs[k]);
            i=j; break;
        }
        if (TempOptArg.empty()) continue;
        TempOptArgs.push_back(TempOptArg);
        TempOptArg.clear();
    }
    return TempOptArgs;
}

void ApiCan::SortOptArgs(vector<vector<string>> &OptArgs)
{
    return;
}

string ApiCan::OutputFormatting(string Str1, string Str2)
{
    size_t OffsetPos,Last,Next,Target;

    if (Str1.length()>=40) {
        Str1.resize(101,' ');
        Str1.append("\n").append(40,' ');
    } else {
        Str1.resize(40,' ');
    }

    if (Str2.length()>60) {
        OffsetPos=59;
        Last=Str2.rfind(' ',OffsetPos);
        Next=Str2.find(' ',OffsetPos);

        Target=string::npos;
        if (Last!=string::npos&&Next!=string::npos) {
            Target=(OffsetPos-Last<=Next-OffsetPos)?Last:Next;
        }
        else if (Last!=string::npos) {
            Target=Last;
        }
        else if (Next!=string::npos) {
            Target=Next;
        }

        if (Target!=string::npos) {
            Str2.insert(Target+1,1,'\n');
            Str2.insert(Target+2,40,' ');
        }
    }
    return Str1 + Str2;
}

void ApiCan::GenHelpInfo(bool isCalled)
{
    int i,Cmds,Opts;
    string TempMsg;
    
    Cmds=(int)s_Cmds.size();
    Opts=(int)s_Opts.size();

    TempMsg.clear();
    TempMsg=DS+s_Cmds[0];
    for (i=1;i<Cmds;i++) TempMsg+="/ "+s_Cmds[i];

    if (!isCalled) TempMsg=this->OutputFormatting(TempMsg,s_Brief);
    else TempMsg=DS+"Command == "+TempMsg;
    this->Cout(TempMsg);

    if (!isCalled) return;
    this->Cout(DS+"Usage : "+s_Cmds[0]+" [-opt]/... [-arg]/...");

    if (Opts) this->Cout("\n  Options : ");
    for (i=0;i<Opts;i++) {
        TempMsg.clear();
        TempMsg+=DS;
        
        // Generate the option type list.
        if (s_Opts[i].OptType&OPTYPE_M) TempMsg+="M";
        else TempMsg+="0";
        if (s_Opts[i].OptType&OPTYPE_D) TempMsg+="D";
        else TempMsg+=" ";
        if (s_Opts[i].OptType&OPTYPE_R) TempMsg+="R";
        else TempMsg+=" ";
        
        TempMsg+=DS+s_Opts[i].LongFmt+"/ "+s_Opts[i].ShortFmt;
        TempMsg=this->OutputFormatting(TempMsg,s_Opts[i].Brief);
        this->Cout(TempMsg);
    }
    return;
}
///    PUBLIC :
/////////////////////////////////////////////////
bool ApiCan::Check(void)
{
    bool State;
        
    State=true;
    if (!this->BasicCheck()) return State=false;
    else if (!this->OptValCheck()) return State=false;
    else if (!this->ArgValCheck()) return State=false;
    
    return State;
}

void *ApiCan::API(void)
{
    return (void *)s_API;
}

void ApiCan::API(vector<string> CmdOptsArgs)
{
    vector<string> OptsArgs;

    // Response to the main help command.
    if (CmdOptsArgs.size()==1) {
        if (CmdOptsArgs[0]=="-help") this->GenHelpInfo(false);
        return;
    }

    // Remove [main cmd] & [-cmd] part.
    CmdOptsArgs.erase(CmdOptsArgs.begin());
    CmdOptsArgs.erase(CmdOptsArgs.begin());
    OptsArgs=CmdOptsArgs;

    // Check the original command.
    if (!this->PreCheck(OptsArgs)) { 
        s_OptArgs.clear();
        return;
    }
    s_OptArgs=this->SplitOpts(OptsArgs);

    // Check the post-process command.
    if (!this->PostCheck(s_OptArgs)) {
        s_OptArgs.clear();
        return;
    }

    // Sort the option arguments by the option list.
    this->SortOptArgs(s_OptArgs);

    s_OptArgs.insert(s_OptArgs.begin(),s_Cmds);
    s_API(s_OptArgs);
    s_OptArgs.clear();

    return;
}

void ApiCan::SetApi(void (*API)(vector<vector<string>>))
{
    s_API=API;

    return;
}

void ApiCan::SetBrief(string Brief)
{
    s_Brief=Brief;

    return;
}

bool ApiCan::ExistCmd(string Cmd)
{
    if (this->CmdIndex(Cmd)!=-1) return true;

    return false;
}

int ApiCan::CmdIndex(string Cmd)
{
    int i,Index,Cmds;
    
    Index=-1;
    Cmds=(int)s_Cmds.size();
    for (i=0;i<Cmds;i++) if (s_Cmds[i]==Cmd) return Index=i;

    return Index;
}

void ApiCan::AppendCmd(string Cmd)
{
    if (!Cmd.empty()&&!this->ExistCmd(Cmd)) s_Cmds.push_back(Cmd);

    return;
}

bool ApiCan::ExistOpt(string OptName)
{
    if (this->OptIndex(OptName)!=-1) return true;

    return false;
}

int ApiCan::OptIndex(string OptName)
{
    int i,Index,Opts;
    
    Index=-1;
    Opts=(int)s_Opts.size();
    for (i=0;i<Opts;i++) {
        if (s_Opts[i].LongFmt==OptName) return Index=i;
        else if (s_Opts[i].ShortFmt==OptName) return Index=i;
    }
    return Index;
}

void ApiCan::AppendOpt(OptFmtData Opt)
{
    int Opts;

    Opts=(int)s_Opts.size();
    if (this->ExistOpt(Opt.LongFmt)) {
        this->StdMsg("you already have [-opt] '"+Opt.LongFmt+"' in [-cmd] '"+s_Cmds[0]+"'",2);
        return;
    }
    else if (this->ExistOpt(Opt.ShortFmt)) {
        this->StdMsg("you already have [-opt] '"+Opt.ShortFmt+"' in [-cmd] '"+s_Cmds[0]+"'",2);
        return;
    }
    s_Opts.push_back(Opt);

    return;
}

/////////////////////////////////////////////////////////////////////
///    CLASS : FData
/////////////////////////////////////////////////////////////////////
FData::FData()
{
    this->Init();
}
FData::~FData()
{
    this->Init();
}
///    PROTECTED :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////
void FData::SetCmdIn(string CmdIn)
{
    if (!CmdIn.empty()) s_CmdIn=CmdIn;

    return;
}

void FData::SetMainCmd(string MainCmd)
{
    if (!MainCmd.empty()) s_MainCmd=MainCmd;

    return;
}

void FData::SetCmdOptArgs(vector<string> CmdOptArgs)
{
    if (!CmdOptArgs.empty()) s_CmdOptsArgs=CmdOptArgs;

    return;
}

vector<string> FData::SplitCmd(string CmdIn)
{
    char Sign;
    int i,j,Size;
    vector<string> CmdSplited;

    Sign=' ';
    Size=(int)CmdIn.size();
    for (i=0;i<Size;i++) {
        if (CmdIn[i]==Sign) continue;
        for (j=i;j<Size;j++) {
            if (CmdIn[j]!=Sign&&j+1!=Size) continue;
            if (CmdIn[j]==Sign) j-=1;
            else if (j+1==Size) j+=0;
            CmdSplited.push_back(CmdIn.substr(i,(j-i)+1));
            i=j; break;
        }
    }
    return CmdSplited;
}

bool FData::ExistCmd(string Cmd)
{
    if (this->CmdIndex(Cmd)!=-1) return true;

    return false;
}

int FData::CmdIndex(string Cmd)
{
    int i,Index,Cmds;
    
    Index=-1;
    Cmds=(int)s_CmdIndex.size();
    for (i=0;i<Cmds;i++) {
        if (s_CmdIndex[i]==Cmd) {
            return Index=i;
        }
    }
    return Index;
}

void FData::AppendCmd(string Cmd)
{
    if (this->ExistCmd(Cmd)) return;
    s_CmdIndex.push_back(Cmd);

    return;
}

bool FData::ExistApiCan(ApiCan ApiCan)
{
    if (this->ApiCanIndex(ApiCan)!=-1) return true; 
 
    return false;
}

int FData::ApiCanIndex(ApiCan ApiCan)
{
    int i,Index,Apis;
    
    Index=-1;
    Apis=(int)s_ApiCanPool.size();
    for (i=0;i<Apis;i++) {
        if (s_ApiCanPool[i].API()==ApiCan.API()) {
            return Index=i;
        }
    }
    return Index;
}

void FData::AppendApiCan(ApiCan ApiCan)
{
    if (this->ExistApiCan(ApiCan)) return;
    s_ApiCanPool.push_back(ApiCan);

    return;
}
///    PRIVATE :
/////////////////////////////////////////////////
void FData::Init(void)
{
    s_CmdIn.clear();
    s_RunSign.clear();
    s_MainCmd.clear();
    s_CmdIndex.clear();
    s_CmdOptsArgs.clear();
    s_ApiCanPool.clear();
    s_CmdApiTable.clear();
    s_ResCmdNum=0;

    return;
}

/////////////////////////////////////////////////////////////////////
///    CLASS : FBuilder
/////////////////////////////////////////////////////////////////////

///
///    USE DEFAULT CONSTRUCTOR
///

///    PROTECTED :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PRIVATE :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////
bool FBuilder::CheckHooks(void)
{
    int i,Cmds;
    bool State;

    State=true;
    Cmds=(int)s_CmdIndex.size();
    for (i=s_ResCmdNum;i<Cmds;i++) {
        if (s_ApiCanPool[s_CmdApiTable[i]].API()==nullptr) {
            this->StdMsg("you are hooking [-cmd] '"+s_CmdIndex[i]+"' with a invaild <ApiCan.API() -> null>",4);
            State=false;
        }
    }
    if (!State) this->StdMsg("please check your command hooks",2);
    
    return State;
}

void FBuilder::HookApi(string ExistCmd,string NewCmd)
{
    int Index;

    Index=this->CmdIndex(ExistCmd);
    if (Index==-1) {
        this->StdMsg("[-cmd] '"+ExistCmd+"' is not exist, you can't hook new command.",2);
        return;
    }
    if (this->ExistCmd(NewCmd)) {
        this->StdMsg("[-cmd] '"+NewCmd+"' already exist, you can't hook as new command.",2);
        return;
    }
    Index=s_CmdApiTable[Index];
    this->AppendCmd(NewCmd);
    s_ApiCanPool[Index].AppendCmd(NewCmd);
    s_CmdApiTable.push_back(Index);
}

void FBuilder::HookApi(string Cmd,void (*API)(vector<vector<string>>))
{
    int Index;
    ApiCan TempCan;

    if (this->ExistCmd(Cmd)) {
        this->StdMsg("[-cmd] '"+Cmd+"'already hooked, current hook will be omited",1);
        return;
    }

    this->AppendCmd(Cmd);
    TempCan.SetApi(API);
    TempCan.AppendCmd(Cmd);
    if (this->ExistApiCan(TempCan)&&API) {
        Index=this->ApiCanIndex(TempCan);
        s_ApiCanPool[Index].AppendCmd(Cmd);
        s_CmdApiTable.push_back(Index);
        return;
    }
    s_ApiCanPool.push_back(TempCan);
    s_CmdApiTable.push_back((int)s_ApiCanPool.size()-1);

    return;
}

/////////////////////////////////////////////////////////////////////
///    CLASS : FParser
/////////////////////////////////////////////////////////////////////

///
///    USE DEFAULT CONSTRUCTOR
///

///    PROTECTED :
/////////////////////////////////////////////////
void FParser::APIhelp(vector<string> OptsArgs)
{
}
void FParser::APIversion(vector<string> OptsArgs)
{
}
void FParser::APIsystem(vector<string> OptsArgs)
{
}
void FParser::APIexit(vector<string> OptsArgs)
{
}
void FParser::ForkReserved(int Index)
{
}
///    PRIVATE :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////
void FParser::ParserCmd(string CmdIn)
{
    int Index;

    this->Cout("");

    if (s_CmdIn.empty()) {                  // REJECT: empty input.
        this->StdMsg("no command input",0);
        return;
    }
    this->SetCmdOptArgs(this->SplitCmd(CmdIn));

    if (s_CmdOptsArgs.empty()) {            // REJECT: empty input.
        this->StdMsg("no command input",0);
    }
    else if (s_CmdOptsArgs[0]!=s_MainCmd) { // REJECT: invaild main command.
        this->StdMsg("invaild command, no '"+s_MainCmd+"'",0);
    }
    else if (s_CmdOptsArgs.size()<2) {      // REJECT: no [-cmd] part.
        this->StdMsg("invaild command, no [-cmd]",0);
    }                                       // REJECT: no [-cmd] matched.
    else if ((Index=this->CmdIndex(s_CmdOptsArgs[1]))<0) { 
        this->StdMsg("invaild command, unknow [-cmd] '"+s_CmdOptsArgs[1]+"'",0);
    }
    else if (Index<s_ResCmdNum) {           // REJECT: reserved command.
        this->ForkReserved(s_CmdApiTable[Index]);
    }
    else {
        this->ForkApi(s_CmdOptsArgs[1]);
    }
    return;
}

void FParser::ForkApi(string Cmd)
{
    int Index;

    Index=this->CmdIndex(Cmd);
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].API(s_CmdOptsArgs);

    return;
}
/////////////////////////////////////////////////////////////////////
///    CLASS : ForgeHwnd
/////////////////////////////////////////////////////////////////////
ForgeHwnd::ForgeHwnd(void)
{
    this->Init();
 
    s_CLIMode=INTRACT_M;
}
ForgeHwnd::ForgeHwnd(int argc,char *argv[])
{
    int i;
    string TempInput;

    this->Init();
    
    for (i=1;i<argc;i++) TempInput+=S+argv[i];
    if (!TempInput.empty()) {
        TempInput+='\n';
        s_HistoryCmd.push_back(TempInput);
    } else {
       s_CLIMode=INTRACT_M;
    }
}
ForgeHwnd::~ForgeHwnd(void)
{
}
///    PROTECTED :
/////////////////////////////////////////////////
void ForgeHwnd::TerminalSet(void)
{
#ifdef _WIN32
    DWORD NewMode;
    HANDLE hStdin;

    hStdin=GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin==INVALID_HANDLE_VALUE) return;
    GetConsoleMode(hStdin,&s_Original);
    NewMode=s_Original;
    NewMode&=~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin,NewMode);
#elif __linux__
    struct termios NewMode;

    tcgetattr(STDIN_FILENO,&s_Original);
    NewMode=s_Original;
    NewMode.c_lflag&=~(ICANON|ECHO);
    NewMode.c_iflag&=~(ICRNL);
    tcsetattr(STDIN_FILENO,TCSANOW,&NewMode);
#endif
    return;
}

void ForgeHwnd::TerminalReset(void)
{
#ifdef _WIN32
    HANDLE hStdin;

    hStdin=GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin!=INVALID_HANDLE_VALUE) {
        SetConsoleMode(hStdin,s_Original);
    }
#elif __linux__
    tcsetattr(STDIN_FILENO,TCSANOW,&s_Original);
#endif
    return;
}

void ForgeHwnd::CmdAutoComplete(string *CurCmd)
{
    int i,Cmds;
    string TempCmd;
    vector<string> TempCmds;
    
    Cmds=(int)s_CmdIndex.size();

    if (CurCmd->empty()) return;

    TempCmds=this->SplitCmd(*CurCmd);
    if (TempCmds.size()!=2) return;

    if ((TempCmd=TempCmds[1]).size()==1) return;

    for (i=0;i<Cmds;i++) {
        if (s_CmdIndex[i].substr(0,TempCmd.size())==TempCmd) {
            cout<<s_CmdIndex[i].substr(TempCmd.size())<<flush;
            *CurCmd+=s_CmdIndex[i].substr(TempCmd.size());

            return;
        }
    }
    return;
}

void ForgeHwnd::InputCmdTask(CmdExchangeData *Data)
{
    size_t p;

    if (Data->ExitFlag) return;

    p=Data->CurInput.find('\n');
    if (p!=string::npos) {
        s_CmdIn.clear();
        this->SetCmdIn(Data->CurInput.substr(0,p));
        Data->CurInput.erase(0,p+1);
        Data->CursorPos=0;
    }
    if (!s_CmdIn.empty()) {
        this->StoreCmd(s_CmdIn);
        this->ParserCmd(s_CmdIn);
        s_CurCmdPos=(int)s_HistoryCmd.size();
    }
#ifdef _WIN32
    Sleep(s_Cfg.InputSleepTime);
#elif __linux__
    usleep(s_Cfg.InputSleepTime*1000);
#endif
    return;
}

void ForgeHwnd::DetecKeyTask(CmdExchangeData *Data)
{
    int KeyVal;

    KeyVal=0;
    while (true) {
        if (!_kbhit()) {
#ifdef _WIN32
            Sleep(s_Cfg.DetectSleepTime);
#elif __linux__
            usleep(s_Cfg.DetectSleepTime*1000);
#endif
            continue;
        }

        KeyVal=_getch();
#ifdef _WIN32
        if (KeyVal==0x00||KeyVal==0xe0) {
            KeyVal=_getch();

            switch (KeyVal) {
            case 0x48: // Up
                this->Refresh(s_RunSign,"");
                this->GetLastCmd(&Data->CurInput);
                Data->CursorPos=(int)Data->CurInput.size();
                break;
            case 0x50: // Down
                this->Refresh(s_RunSign,"");
                this->GetNextCmd(&Data->CurInput);
                Data->CursorPos=(int)Data->CurInput.size();
                break;
            case 0x4d: // Right
                if (Data->CursorPos<(int)Data->CurInput.size()) {
                    Data->CursorPos++;
                }
                break;
            case 0x4b: // Left
                if (Data->CursorPos>0) {
                    Data->CursorPos--;
                }
                break;
            }
        }
#elif __linux__
        if (KeyVal==0x7f) KeyVal=0x08;
        if (KeyVal==0x1b) {

            if (!_kbhit()) Data->ExitFlag=true;

            KeyVal=_getch();

            if (KeyVal==0x5b&&_kbhit()) {
                KeyVal=_getch();

                switch (KeyVal) {
                case 0x41: // Up
                    this->Refresh(s_RunSign,"");
                    this->GetLastCmd(&Data->CurInput);
                    Data->CursorPos=(int)Data->CurInput.size();
                    break;
                case 0x42: // Down
                    this->Refresh(s_RunSign,"");
                    this->GetNextCmd(&Data->CurInput);
                    Data->CursorPos=(int)Data->CurInput.size();
                    break;
                case 0x43: // Right
                    if (Data->CursorPos<(int)Data->CurInput.size()) {
                        Data->CursorPos++;
                    }
                    break;
                case 0x44: // Left
                    if (Data->CursorPos>0) {
                        Data->CursorPos--;
                    }
                    break;
                }
            }
        }
#endif
        else {
            switch (KeyVal) {
            case 0x08: // Backspace
                if (Data->CursorPos>0) {
                    Data->CurInput.erase(Data->CursorPos-1,1);
                    Data->CursorPos--;
                }
                break;
            case 0x09: // Tab
                this->CmdAutoComplete(&Data->CurInput);
                Data->CursorPos=(int)Data->CurInput.size();
                break;
            case 0x0d: // Enter
                Data->CurInput+='\n';
                return;
            case 0x1b: // Esc
                this->Cout("\n");
                Data->ExitFlag=true;
                return;
            default:   // Character
                if (KeyVal<32||KeyVal>126) continue;
                Data->CurInput.insert(Data->CursorPos,1,KeyVal);
                Data->CursorPos++;
                break;
            }
        }
        // Update console
        this->Refresh(s_RunSign,Data->CurInput);
        this->CurMove((int)(s_RunSign.size())+Data->CursorPos);
    }
    return;
}

void ForgeHwnd::APIhelp(vector<string> OptsArgs)
{
    this->GenHelpInfo();
}

void ForgeHwnd::APIversion(vector<string> OptsArgs)
{
    this->GenVersionInfo();
}

void ForgeHwnd::APIsystem(vector<string> OptsArgs)
{
    int i;
    string TempCmd;

    if (OptsArgs.size()<3) return;
    for (i=2;i<(int)OptsArgs.size();i++) {
        TempCmd+=OptsArgs[i]+S;
    }
    system(TempCmd.data());
}

void ForgeHwnd::APIexit(vector<string> OptsArgs)
{
    exit(0);
}

void ForgeHwnd::ForkReserved(int Index)
{
    // Reserved command:
    // --help    : generate help message of all api.
    // --system  : reserved system interface.
    // --version : generate version information of this program.
    // --exit    : exit this program.
    //
    // YOU CAN ADD YOUR OWN RESERVED COMMAND HERE.
    switch (Index) {
    case 0: // -h/--help
        this->APIhelp(s_CmdOptsArgs); break;
    case 1: // -v/--version
        this->APIversion(s_CmdOptsArgs); break;
    case 2: // -sys/--system
        this->APIsystem(s_CmdOptsArgs); break;
    case 3: // -ext/--exit
        this->APIexit(s_CmdOptsArgs); break;
    default:
        break;
    }
    return;
}
///    PRIVATE :
/////////////////////////////////////////////////
void ForgeHwnd::Init(void)
{
    // Init
    s_CLIMode=ONELINE_M;
    s_CurCmdPos=0;
    s_HistoryCmd.clear();

    // Init of config
    s_Cfg.Version.clear();
    s_Cfg.ProgramName.clear();
    s_Cfg.MaxStoredCmd=10;
    s_Cfg.InputSleepTime=30;
    s_Cfg.DetectSleepTime=30;

    // These is the reserved command:
    this->HookApi("-h",nullptr);
    this->HookApi("-h","--help");
    this->HookApi("-v",nullptr);
    this->HookApi("-v","--version");
    this->HookApi("-sys",nullptr);
    this->HookApi("-sys","--system");
    this->HookApi("-ext",nullptr);
    this->HookApi("-ext","--exit");

    this->SetCmdBrief("-h","get help infomation");
    this->SetCmdBrief("-v","get version message");
    this->SetCmdBrief("-sys","system interface");
    this->SetCmdBrief("-ext","quit the program");

    s_ResCmdNum=(int)s_CmdIndex.size();
    s_ResApiNum=(int)s_ApiCanPool.size();
}

bool ForgeHwnd::Check(void)
{
    bool State;
    int i,Apis;

    State=true;
    Apis=(int)s_ApiCanPool.size();
    if (!this->CheckHooks()) State=false;

    for (i=s_ResApiNum;i<Apis;i++) {
        if (!s_ApiCanPool[i].Check()) State=false;
    }
    return State;
}

void ForgeHwnd::StoreCmd(string CurCmd)
{
    if ((int)s_HistoryCmd.size()>=s_Cfg.MaxStoredCmd) {
        s_HistoryCmd.pop_front();
    }
    s_HistoryCmd.push_back(CurCmd);

    return;
}

void ForgeHwnd::GetLastCmd(string *CurCmd)
{
    if (s_HistoryCmd.empty()) return;
    if (s_CurCmdPos==0) return;

    *CurCmd=s_HistoryCmd[--s_CurCmdPos];

    return;
}

void ForgeHwnd::GetNextCmd(string *CurCmd)
{
    if (s_HistoryCmd.empty()) return;
    if (s_CurCmdPos==(int)s_HistoryCmd.size()-1) return;

    *CurCmd=s_HistoryCmd[++s_CurCmdPos];

    return;
}

void ForgeHwnd::GenHelpInfo(void)
{
    int i,Apis;
    
    Apis=(int)s_ApiCanPool.size();
    this->Cout(DS+"Usage : "+s_MainCmd+" [-cmd] [-opt]/... [-arg]/...\n");

    this->Cout(DS+"Commands : ");
    for (i=0;i<Apis;i++) {
        s_ApiCanPool[i].API({"-help"});
    }
    return;
}

void ForgeHwnd::GenVersionInfo(void)
{
    this->Cout("___________________________________________");
    this->Cout("  "+s_Cfg.ProgramName                       );
    this->Cout("  Version: "+s_Cfg.Version                  );
    this->Cout("");
    this->Cout("____________Powered by CmdForge____________");
    this->Cout("");
    
    return;
}
///    PUBLIC :
/////////////////////////////////////////////////
void ForgeHwnd::SetCLICfg(CLICfgData Cfg)
{
    s_Cfg=Cfg;

    return;
}

void ForgeHwnd::SetCLIMode(int Mode)
{
    int InputSleepTime, DetectSleepTime, MaxStoredCmd;

    switch (Mode)
    {
    case DEFAULT_M:MaxStoredCmd=30;
        InputSleepTime=DetectSleepTime=30;
        break;
    case HIGHPFM_M:MaxStoredCmd=50;
        InputSleepTime=DetectSleepTime=10;
        break;
    case BALANCE_M:MaxStoredCmd=20;
        InputSleepTime=DetectSleepTime=50;
        break;
    case LOWCOST_M:MaxStoredCmd=10;
        InputSleepTime=DetectSleepTime=100;
        break;
    default:MaxStoredCmd=10;
        InputSleepTime=DetectSleepTime=30;
        break;
    }
    s_Cfg.MaxStoredCmd=MaxStoredCmd;
    s_Cfg.InputSleepTime=InputSleepTime;
    s_Cfg.DetectSleepTime=DetectSleepTime;

    return;
}

void ForgeHwnd::SetCLIVersion(string Version)
{
    s_Cfg.Version=Version;

    return;
}

void ForgeHwnd::SetCLIMainCmd(string MainCmd)
{
    this->SetMainCmd(MainCmd);

    return;
}

void ForgeHwnd::HookCmdApi(string ExistCmd, string NewCmd)
{
    this->HookApi(ExistCmd,NewCmd);

    return;
}

void ForgeHwnd::HookCmdApi(string Cmd, void (*API)(vector<vector<string>>))
{
    this->HookApi(Cmd,API);

    return;
}

void ForgeHwnd::SetCmdBrief(string Cmd,string Brief)
{
    int Index;

    if ((Index=this->CmdIndex(Cmd))==-1) {
        this->StdMsg("You are setting brief to an unhooked [-cmd] '"+Cmd+"'", 4);
        return;
    }
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].SetBrief(Brief);

    return;
}

void ForgeHwnd::SetCmdOpt(string Cmd,OptFmtData OptFmt)
{
    int Index;

    if ((Index=this->CmdIndex(Cmd))==-1) {
        this->StdMsg("You are setting options to an unhooked [-cmd] '"+Cmd+"'",4);
        return;
    }
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].AppendOpt(OptFmt);

    return;
}

int ForgeHwnd::MainLoop(string RunSign)
{
    bool isIntract;
    CmdExchangeData Data;

    isIntract=(s_CLIMode==INTRACT_M);

    s_RunSign=RunSign;
    Data.ExitFlag=false;
    Data.CurInput.clear();
    Data.CurInput=isIntract?"":s_MainCmd+s_HistoryCmd[0];
    Data.CursorPos=isIntract?0:(int)Data.CurInput.length();

    if (!this->Check()) return 0;

    this->TerminalSet();
    if (!isIntract) {             // Mode : command oneline.
        this->InputCmdTask(&Data);
    } else {                      // Mode : command interact.
        while (!Data.ExitFlag) {
            this->Refresh(s_RunSign,"");
            this->DetecKeyTask(&Data);
            this->InputCmdTask(&Data);
        }
    }
    this->TerminalReset();

    return 0;
}