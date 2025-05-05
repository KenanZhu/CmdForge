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
//  VERSION OF LIB    : 1.0.9
// ----------------------------------------------------------------------------

#include "CmdForge.h"

#ifdef _DEBUG
void SetDebug(bool IsDebug)
{
    g_IsDebug=IsDebug;
}

bool GetDebug(void)
{
    return g_IsDebug;
}
#endif // _DEBUG

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
void SysOut::Cout(const string &Msg,int Endl)
{
    if (Endl==0) cout<<Msg<<flush;
    if (Endl==1) cout<<Msg<<endl<<flush;

    return;
}

void SysOut::StdMsg(const string &Msg,int Level)
{
    string LevelSign;

#ifdef _DEBUG
    if (!GetDebug()&&Level) return;
#else
    if (Level) return;
#endif // _DEBUG

    switch (Level) {
    case 0:LevelSign=DS; break;

    // _DEBUG : log sign when enable debug.
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

    this->Cout("\033[s",0);
    this->Cout("\033["+to_string(NewPos+1)+"G",0);

    return;
}

void SysOut::Refresh(string &RunSign,string CurCmd)
{
    this->SetInputLength((int)(RunSign.size()+CurCmd.size()));

    this->Cout("\33[2K\r"+RunSign+CurCmd,0);

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
bool ApiCan::PreParserCheck(vector<string> &OptsArgs)
{
    int i,Opts;
    bool State;
    
    State=true;
    Opts=(int)s_Opts.size();

    // CHECK: no [-opt] part.
    if (OptsArgs.size()==0) {
        if (!Opts) return State=true;

        this->StdMsg("no [-opt] part",0);
        this->StdMsg("need help? use [-opt] '-help/-h'",0);
        State=false;
    }
    // CHECK: '-help/-h' [-opt].
    else if (OptsArgs[0]=="-help"||OptsArgs[0]=="-h") {
        this->GenHelpInfo(true);
        State=false;
    }
    // CHECK: use default [-opt].
    else if (!this->ExistOpt(OptsArgs[0])) {
        for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_D) {
            OptsArgs.insert(OptsArgs.begin(),s_Opts[0].LongFmt);
        }
        State=true;
    }
    return State;
}

bool ApiCan::PostParserCheck(vector<vector<string>> &OptArgs)
{
    bool State;
    int i,Index,ExpArgs,ActArgs,ExpOpts,ActOpts,RepeatTimes,Size,Opts;
    unordered_multiset<string> InputOpts;

    State=true;
    ExpOpts=ActOpts=0;
    Size=(int)OptArgs.size();
    Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_M) ExpOpts++;
    for (i=0;i<Size;i++) InputOpts.insert(OptArgs[i][0]);

    // Check the options.
    //
    // 1. It will check if lost mandatory options.
    // 2. It will check if the unrepeatable option is repeat.
    if (Opts&&Size==0) {this->StdMsg("no valid [-opt].",0); return State=false;}

    // 1.
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

    // 2.
    for (i=0;i<Opts;i++) {
        if (s_Opts[i].OptType&OPTYPE_R) continue;

        RepeatTimes=(int)(InputOpts.count(s_Opts[i].LongFmt)+InputOpts.count(s_Opts[i].ShortFmt));
        if (RepeatTimes>1) {
            this->StdMsg("got "+to_string(RepeatTimes)+" [-opt](s) '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"' in command, but it is unrepeatable",0);
            State=false;
        }
    }

    // Check the arguments.
    // :
    // *The OptArgs is split by exist option, so no need to check option.
    // 
    // 1. It will check the amount of received argument is equal with the expect.

    // 1.
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

bool ApiCan::BasicBuildCheck(void)
{
    bool State;
    int Opts,Cmds;
    
    State=true;
    Cmds=(int)s_Cmds.size();
    Opts=(int)s_Opts.size();

    if (s_API==nullptr) {
        this->StdMsg("no api was hooked in a api can",3);
        return State=false;
    }
    else if (Cmds==0) {
        this->StdMsg("no [-cmd] was hooked in a api can",3);
        return State=false;
    }
    else if (Opts==0) {
        this->StdMsg("no [-opt] include in [-cmd] '"+s_Cmds[0]+"'",2);
        return State=true;
    }
    return State;
}

bool ApiCan::OptBuildCheck(void)
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
        this->StdMsg("[-cmd] '"+s_Cmds[0]+"' expect only 1 default [-opt], but you set "+to_string(ActDefault)+" default [-opt]",2);
        return State=false;
    }
    return State;
}

bool ApiCan::ArgBuildCheck(void)
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

vector<vector<string>> ApiCan::SplitOpts(vector<string> &OptsArgs)
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

            // Store the arguments.
            for (k=i;k<=j;k++) TempOptArg.push_back(OptsArgs[k]);
            i=j; break;
        }
        if (TempOptArg.empty()) continue;
        TempOptArgs.push_back(TempOptArg);
        TempOptArg.clear();
    }
    return TempOptArgs;
}

string ApiCan::FormatTextColumns(const string &Str1,const string &Str2,int LeftWidth,int RightWidth)
{
    size_t Target,Last,Next;
    string TempStr1,TempStr2,Result;
    deque<string> Str1Lines,Str2Lines;

    TempStr1=Str1;
    TempStr2=Str2;
    while ((int)TempStr1.size()>LeftWidth+(int)RightWidth/10) {

        Next=TempStr1.find(' ',LeftWidth);
        Last=TempStr1.rfind(' ',LeftWidth);
        Target=min(Next,Last);
        Target=Target==string::npos?LeftWidth:Target;
        Str1Lines.push_back(TempStr1.substr(0,Target)+'\n');
        TempStr1.erase(0,Target+1);
    }
    (int)TempStr1.size()<=LeftWidth?TempStr1.resize(LeftWidth,' '):TempStr1.resize(LeftWidth+RightWidth,' ');
    Str1Lines.push_back(TempStr1);

    while ((int)TempStr2.size()>RightWidth) {

        Next=TempStr2.find(' ',RightWidth);
        Last=TempStr2.rfind(' ',RightWidth);
        Target=min(Next,Last);
        Target=Target==string::npos?LeftWidth:Target;
        Str2Lines.push_back(TempStr2.substr(0,Target)+'\n');
        TempStr2.erase(0,Target+1);
    }
    Str2Lines.push_back(TempStr2);

    while ((int)Str1Lines.size()!= 1) {

        Result+=DS+Str1Lines[0];
        Str1Lines.pop_front();
    }
    if ((int)Str1Lines[0].size()>LeftWidth) {
        Result+=DS+Str1Lines[0]+"\n";
    }
    else {
        Result+=DS+Str1Lines[0]+" "+Str2Lines[0];
        Str2Lines.pop_front();
    }
    Str1Lines.pop_front();

    while ((int)Str2Lines.size()) {

        Result+=DS+Str2Lines[0].insert(0,LeftWidth+1,' ');
        Str2Lines.pop_front();
    }
    return Result;
}

void ApiCan::GenHelpInfo(bool isCalled)
{
    int i,Cmds,Opts;
    string TempMsg;
    
    Cmds=(int)s_Cmds.size();
    Opts=(int)s_Opts.size();

    TempMsg.clear();
    TempMsg=s_Cmds[0];
    for (i=1;i<Cmds;i++) TempMsg+=", "+s_Cmds[i];

    if (!isCalled) TempMsg=this->FormatTextColumns(TempMsg,s_Brief,20,70);
    else TempMsg=DS+"Command : "+TempMsg;
    this->Cout(TempMsg);

    if (!isCalled) return;
    this->Cout(DS+"Usage : "+s_Cmds[0]+" [-opt], ... [-arg], ...");

    if (Opts) {
        this->Cout("");
        this->Cout(DS+"Options : ");
    }
    for (i=0;i<Opts;i++) {
        TempMsg.clear();
        
        // Generate the option type list.
        if (s_Opts[i].OptType&OPTYPE_D) TempMsg+="D";
        else TempMsg+=" ";
        if (s_Opts[i].OptType&OPTYPE_M) TempMsg+="M";
        else TempMsg+="0";
        if (s_Opts[i].OptType&OPTYPE_R) TempMsg+="R";
        else TempMsg+=" ";
        
        TempMsg+=DS+s_Opts[i].LongFmt+"/ "+s_Opts[i].ShortFmt;
        TempMsg=this->FormatTextColumns(TempMsg,s_Opts[i].Brief,30,60);
        this->Cout(TempMsg);
    }
    return;
}
///    PUBLIC :
/////////////////////////////////////////////////
bool ApiCan::BuildCheck(void)
{
    bool State;
        
    State=true;
    if (!this->BasicBuildCheck()) return State=false;
    else if (!this->OptBuildCheck()) return State=false;
    else if (!this->ArgBuildCheck()) return State=false;
    
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
    if (!this->PreParserCheck(OptsArgs)) { 
        s_OptArgs.clear();
        return;
    }
    s_OptArgs=this->SplitOpts(OptsArgs);

    // Check the post-process command.
    if (!this->PostParserCheck(s_OptArgs)) {
        s_OptArgs.clear();
        return;
    }
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

void ApiCan::SetBrief(const string &Brief)
{
    s_Brief=Brief;

    return;
}

bool ApiCan::ExistCmd(const string &Cmd)
{
    if (this->CmdIndex(Cmd)!=-1) return true;

    return false;
}

int ApiCan::CmdIndex(const string &Cmd)
{
    int i,Index,Cmds;
    
    Index=-1;
    Cmds=(int)s_Cmds.size();
    for (i=0;i<Cmds;i++) if (s_Cmds[i]==Cmd) return Index=i;

    return Index;
}

void ApiCan::AppendCmd(const string &Cmd)
{
    if (!Cmd.empty()&&!this->ExistCmd(Cmd)) s_Cmds.push_back(Cmd);

    return;
}

bool ApiCan::ExistOpt(const string &OptName)
{
    if (this->OptIndex(OptName)!=-1) return true;

    return false;
}

int ApiCan::OptIndex(const string &OptName)
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

vector<string> ApiCan::GetAllOpts(void)
{
    vector<string> TempOpts;

    for (const OptFmtData &Opt:s_Opts) {
        TempOpts.push_back(Opt.LongFmt);
        TempOpts.push_back(Opt.ShortFmt);
    }
    return TempOpts;
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
void FData::SetCmdIn(const string &CmdIn)
{
    vector<string> TempOptArgs;

    if (CmdIn.empty()) return;

    s_CmdIn=CmdIn;
    TempOptArgs=this->SplitCmd(s_CmdIn);
    if (TempOptArgs.empty()) {
        s_CmdIn.clear();
        s_CmdOptsArgs.clear();
    }
    this->SetCmdOptsArgs(TempOptArgs);

    return;
}

string FData::GetCmdIn(void)
{
    return s_CmdIn;
}

void FData::SetMainCmd(const string &MainCmd)
{
    if (!MainCmd.empty()) s_MainCmd=MainCmd;

    return;
}

string FData::GetMainCmd(void)
{
    return s_MainCmd;
}

void FData::SetCmdOptsArgs(vector<string> &CmdOptsArgs)
{
    if (!CmdOptsArgs.empty()) s_CmdOptsArgs=CmdOptsArgs;

    return;
}

vector<string> FData::GetCmdOptsArgs(void)
{
    return s_CmdOptsArgs;
}

vector<string> FData::SplitCmd(string &CmdIn)
{
    char Sign;
    int i,j,Size;
    vector<string> CmdSplit;

    Sign=' ';
    Size=(int)CmdIn.size();
    for (i=0;i<Size;i++) {
        if (CmdIn[i]==Sign) continue;
        for (j=i;j<Size;j++) {
            if (CmdIn[j]!=Sign&&j+1!=Size) continue;
            if (CmdIn[j]==Sign) j-=1;
            else if (j+1==Size) j+=0;
            CmdSplit.push_back(CmdIn.substr(i,(j-i)+1));
            i=j; break;
        }
    }
    return CmdSplit;
}

bool FData::ExistCmd(const string &Cmd)
{
    if (this->CmdIndex(Cmd)!=-1) return true;

    return false;
}

int FData::CmdIndex(const string &Cmd)
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

void FData::AppendCmd(const string &Cmd)
{
    if (this->ExistCmd(Cmd)) return;
    s_CmdIndex.push_back(Cmd);
    this->SortCmdIndex();

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

bool FData::CompareStrAlpha(const string &Str1,const string &Str2)
{
    char ca,cb;
    size_t i,j;

    i=j=0;
    while (true) {
        while (i<Str1.size()&&!std::isalpha(Str1[i])) ++i;
        while (j<Str2.size()&&!std::isalpha(Str2[j])) ++j;

        if (i>=Str1.size()&&j>=Str2.size()) return false;
        if (i>=Str1.size()) return true;
        if (j>=Str2.size()) return false;

        ca=tolower(Str1[i]);
        cb=tolower(Str2[j]);
        if (ca!=cb) return ca<cb;
        ++i; ++j;
    }
}

void FData::SortStrList(vector<string> &SourceStrList)
{
    sort(SourceStrList.begin(),SourceStrList.end(),&FData::CompareStrAlpha);
}

void FData::SortCmdIndex(void)
{
    s_SortedCmdIndex=s_CmdIndex;
    sort(s_SortedCmdIndex.begin(),s_SortedCmdIndex.end(),&FData::CompareStrAlpha);
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
            this->StdMsg("you are hooking [-cmd] '"+s_CmdIndex[i]+"' with a invalid <ApiCan.API() -> null>",3);
            State=false;
        }
    }
    if (!State) this->StdMsg("please check your command hooks",2);
    
    return State;
}

void FBuilder::HookApi(const string &ExistCmd,const string &NewCmd)
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
    this->AppendCmd(NewCmd);
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].AppendCmd(NewCmd);
    s_CmdApiTable.push_back(Index);
}

void FBuilder::HookApi(const string &Cmd,void (*API)(vector<vector<string>>))
{
    int Index;
    ApiCan TempCan;

    if (this->ExistCmd(Cmd)) {
        this->StdMsg("[-cmd] '"+Cmd+"' already hooked, current hook will be omitted",1);
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
void FParser::APIhelp(vector<string> &OptsArgs)
{
}
void FParser::APIversion(vector<string> &OptsArgs)
{
}
void FParser::APIsystem(vector<string> &OptsArgs)
{
}
void FParser::APIexit(vector<string> &OptsArgs)
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
void FParser::ParserCmd(void)
{
    int Index;
    string TempMainCmd;
    vector<string> TempCmdOptsArgs;

    this->Cout("");

    // REJECT: empty input.
    if (this->GetCmdIn().empty()) {
        this->StdMsg("no command input",0);
        return;
    }
    TempMainCmd=this->GetMainCmd();
    TempCmdOptsArgs=this->GetCmdOptsArgs();

    // REJECT: empty input.
    if (TempCmdOptsArgs.empty()) {
        this->StdMsg("no command input",0);
    }
    // REJECT: invalid main command.
    else if (TempCmdOptsArgs[0]!=TempMainCmd) {
        this->StdMsg("invalid command, no '"+TempMainCmd+"'",0);
    }
    // REJECT: no [-cmd] section.
    else if (TempCmdOptsArgs.size()<2) {
        this->StdMsg("invalid command, no [-cmd]",0);
    }                                       
    // REJECT: no [-cmd] matched.
    else if ((Index=this->CmdIndex(TempCmdOptsArgs[1]))<0) {
        this->StdMsg("invalid command, unknow [-cmd] '"+TempCmdOptsArgs[1]+"'",0);
    }
    // ACCEPT
    else {
        this->ForkApi(TempCmdOptsArgs[1]);
    }
    return;
}

void FParser::ForkApi(const string &Cmd)
{
    int Index;

    Index=this->CmdIndex(Cmd);
    Index=s_CmdApiTable[Index];
    if (Index<s_ResApiNum) this->ForkReserved(Index);
    else s_ApiCanPool[Index].API(s_CmdOptsArgs);

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
        this->AppendCmdIn(TempInput);
    } else {
       s_CLIMode=INTRACT_M;
    }
}
ForgeHwnd::~ForgeHwnd(void)
{
}
///    PROTECTED :
/////////////////////////////////////////////////
void ForgeHwnd::SetTerminal(void)
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

void ForgeHwnd::ResetTerminal(void)
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

void ForgeHwnd::Complete(string &SourceStr, vector<string> &TargetStrList)
{
    int Index;
    string FiltSrcStr;
    string FiltTgtStr;
    vector<string> MatchTgtStrList;

    // Filter out non-alphabetic characters from SourceStr.
    auto AlphaFilter=[](const string &Str)
    {
        string Res;

        for (char C:Str) {
            if (std::isalpha(C)) Res+=C; 
        }
        return Res;
    };
    FiltSrcStr=AlphaFilter(SourceStr);
    if (FiltSrcStr.empty()) return;

    // Find all the string in TargetStrList that partly or totally
    // match with the filtered SourceStr.
    Index=0;
    for (string &TargetStr:TargetStrList) {
        Index++;
        FiltTgtStr=AlphaFilter(TargetStr);
        if (FiltTgtStr.compare(0,FiltSrcStr.size(),FiltSrcStr)==0||
            FiltSrcStr.compare(0,FiltTgtStr.size(),FiltTgtStr)==0) {
            if (FiltSrcStr==FiltTgtStr) {
                MatchTgtStrList.clear();
                MatchTgtStrList.push_back(TargetStr);
                break;
            }
            MatchTgtStrList.push_back(TargetStr);
        }
    }
    if (MatchTgtStrList.empty()) return;

    if ((int)MatchTgtStrList.size()==1&&MatchTgtStrList[0]==SourceStr) {
        if (Index<(int)TargetStrList.size()) {
            SourceStr=TargetStrList[Index];
        } else {
            SourceStr=TargetStrList[0];
        }
        return;
    }
    SourceStr=MatchTgtStrList[0];

    return;
}

void ForgeHwnd::CompleteOpt(string &CurCmd, string &CurOpt)
{
    int Index;
    vector<string> Opts;

    Index=this->CmdIndex(CurCmd);
    if (Index==-1) return;
    Index=s_CmdApiTable[Index];
    Opts=s_ApiCanPool[Index].GetAllOpts();
    this->SortStrList(Opts);

    this->Complete(CurOpt,Opts);

    return;
}

void ForgeHwnd::CompleteCmd(string &CurCmd)
{
    this->Complete(CurCmd,s_SortedCmdIndex);
    
    return;
}

void ForgeHwnd::AutoCompleteCmd(string &CurCmd)
{
    int i,Cmds;
    string TempCmd;
    vector<string> TempCmds;
    
    Cmds=(int)s_CmdIndex.size();
    TempCmds=this->SplitCmd(CurCmd);

    // COMP : no input, complete the main command.
    if (TempCmds.empty()) {
        TempCmds.push_back(this->GetMainCmd());
    }
    // COMP : complete the main command.
    else if ((int)TempCmds.size()<=1) {
        TempCmds[0]=this->GetMainCmd();
    }
    // COMP : complete the sub-command.
    else if ((int)TempCmds.size()<=2) {
        this->CompleteCmd(TempCmds[1]);
    }
    // COMP : complete the option by the command.
    else {
        this->CompleteOpt(TempCmds[1],TempCmds.back());
    }
    TempCmd.clear();
    for (i=0;i<(int)TempCmds.size();i++) TempCmd+=TempCmds[i]+S;
    CurCmd=TempCmd;

    return;
}

void ForgeHwnd::InputCmdTask(CmdExchangeData *Data)
{
    size_t p;

    if (Data->ExitFlag) return;

    p=Data->CurInput.find('\n');
    if (p!=string::npos) Data->CurInput.replace(p,1,";");

    while ((p=Data->CurInput.find(';'))!=string::npos) {
        this->SetCmdIn(Data->CurInput.substr(0, p));
        Data->CurInput.erase(0,p+1);
        Data->CursorPos=0;
        
        if (this->GetCmdIn().empty()) continue;
        this->AppendCmdIn(this->GetCmdIn());
        this->ParserCmd();
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
                this->GetLastCmdIn(Data->CurInput);
                Data->CursorPos=(int)Data->CurInput.size();
                break;
            case 0x50: // Down
                this->Refresh(s_RunSign,"");
                this->GetNextCmdIn(Data->CurInput);
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
            case 0x53: // Delete
                if (Data->CursorPos<=(int)Data->CurInput.size()) {
                    Data->CurInput.erase(Data->CursorPos,1);
                }
                break;
            }
        }
#elif __linux__
        if (KeyVal==0x7f) KeyVal=0x08;
        if (KeyVal==0x1b) {
            if (!_kbhit())  {
                this->Cout("\n");
                Data->ExitFlag=true;
                return;
            }
            if ((KeyVal=_getch())==0x5b&&_kbhit()) {
                KeyVal=_getch();

                switch (KeyVal) {
                case 0x41: // Up
                    this->Refresh(s_RunSign,"");
                    this->GetLastCmdIn(Data->CurInput);
                    Data->CursorPos=(int)Data->CurInput.size();
                    break;
                case 0x42: // Down
                    this->Refresh(s_RunSign,"");
                    this->GetNextCmdIn(Data->CurInput);
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
                case 0x33: // Delete
                    if (Data->CursorPos<=(int)Data->CurInput.size()) {
                        Data->CurInput.erase(Data->CursorPos,1);
                        while (_kbhit()) _getch();
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
                this->AutoCompleteCmd(Data->CurInput);
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

void ForgeHwnd::APIhelp(vector<string> &OptsArgs)
{
    this->GenHelpInfo();
}

void ForgeHwnd::APIversion(vector<string> &OptsArgs)
{
    this->GenVersionInfo();
}

void ForgeHwnd::APIsystem(vector<string> &OptsArgs)
{
    int i;
    string TempCmd;

    if (OptsArgs.size()<3) return;
    for (i=2;i<(int)OptsArgs.size();i++) {
        TempCmd+=OptsArgs[i]+S;
    }
    system(TempCmd.data());
}

void ForgeHwnd::APIexit(vector<string> &OptsArgs)
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

bool ForgeHwnd::BuildCheck(void)
{
    bool State;
    int i,Apis;

    State=true;
    Apis=(int)s_ApiCanPool.size();
    if (!this->CheckHooks()) State=false;

    for (i=s_ResApiNum;i<Apis;i++) {
        if (!s_ApiCanPool[i].BuildCheck()) State=false;
    }
    return State;
}

void ForgeHwnd::AppendCmdIn(const string &CurCmd)
{
    if ((int)s_HistoryCmd.size()>=s_Cfg.MaxStoredCmd) {
        s_HistoryCmd.pop_front();
    }
    s_HistoryCmd.push_back(CurCmd);
    s_CurCmdPos=(int)s_HistoryCmd.size();

    return;
}

void ForgeHwnd::GetLastCmdIn(string &CurCmdIn)
{
    if (s_HistoryCmd.empty()) return;
    if (s_CurCmdPos<=0) return;

    CurCmdIn=s_HistoryCmd[--s_CurCmdPos];

    return;
}

void ForgeHwnd::GetNextCmdIn(string &CurCmdIn)
{
    if (s_HistoryCmd.empty()) return;
    if (s_CurCmdPos>=(int)s_HistoryCmd.size()-1) {
        s_CurCmdPos=(int)s_HistoryCmd.size();
        CurCmdIn.clear();
        return;
    }
    CurCmdIn=s_HistoryCmd[++s_CurCmdPos];

    return;
}

void ForgeHwnd::GenHelpInfo(void)
{
    int i,Apis;
    
    Apis=(int)s_ApiCanPool.size();
    this->Cout(DS+"Usage : "+this->GetMainCmd()+" [-cmd] [-opt], ... [-arg], ...\n");

    this->Cout(DS+"Commands : ");
    for (i=0;i<Apis;i++) {
        s_ApiCanPool[i].API({"-help"});
    }
    return;
}

void ForgeHwnd::GenVersionInfo(void)
{
    this->Cout("");
    this->Cout("  "+s_Cfg.ProgramName                       );
    this->Cout("  Version: "+s_Cfg.Version                  );
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

void ForgeHwnd::SetCLIVersion(const string &Version)
{
    s_Cfg.Version=Version;

    return;
}

void ForgeHwnd::SetCLIMainCmd(const string &MainCmd)
{
    this->SetMainCmd(MainCmd);

    return;
}

OptFmtData ForgeHwnd::GenOptFmt(const string &LongFmt,const string &ShortFmt,const string &Brief,const vector<ArgFmtData> &Args,int OptType)
{
    OptFmtData TempOpt;

    TempOpt.Brief=Brief;
    TempOpt.LongFmt=LongFmt;
    TempOpt.ShortFmt=ShortFmt;
    TempOpt.OptType=OptType;
    TempOpt.Args=Args;

    return TempOpt;
}

void ForgeHwnd::SetCmdBrief(const string &Cmd,const string &Brief)
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

void ForgeHwnd::SetCmdOpt(const string &Cmd,const OptFmtData &Opt)
{
    int Index;

    if ((Index=this->CmdIndex(Cmd))==-1) {
        this->StdMsg("You are setting options to an unhooked [-cmd] '"+Cmd+"'",4);
        return;
    }
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].AppendOpt(Opt);

    return;
}

void ForgeHwnd::SetCmdOpts(const string &Cmd,const vector<OptFmtData> &Opts)
{
    for(const OptFmtData &Opt:Opts) {
        this->SetCmdOpt(Cmd,Opt);
    }
    return;
}

void ForgeHwnd::HookCmdApi(const string &ExistCmd,const string &NewCmd)
{
    this->HookApi(ExistCmd,NewCmd);

    return;
}

void ForgeHwnd::HookCmdApi(const string &Cmd,void (*API)(vector<vector<string>>))
{
    this->HookApi(Cmd,API);

    return;
}

void ForgeHwnd::HookCmdApi(const string &Cmd,const string &Brief,const vector<OptFmtData> &Opts,void(*API)(vector<vector<string>>))
{
    this->HookApi(Cmd,API);
    this->SetCmdBrief(Cmd,Brief);
    this->SetCmdOpts(Cmd,Opts);

    return;
}

int ForgeHwnd::MainLoop(const string &RunSign)
{
    bool isInteract;
    CmdExchangeData Data;

    isInteract=(s_CLIMode==INTRACT_M);

    s_RunSign=RunSign;
    Data.ExitFlag=false;
    Data.CurInput.clear();
    Data.CurInput=isInteract?"":s_MainCmd+s_HistoryCmd[0];
    Data.CursorPos=isInteract?0:(int)Data.CurInput.length();

    if (!this->BuildCheck()) {
        this->Cout("\nCmdForge encountered some errors, build terminated !\n");
        return 0;
    }

    this->SetTerminal();
    if (!isInteract) {
    // Mode : command oneline.

        this->InputCmdTask(&Data);
    } else {
    // Mode : command interact.

        while (!Data.ExitFlag) {
            this->Refresh(s_RunSign,"");
            this->DetecKeyTask(&Data);
            this->InputCmdTask(&Data);
        }
    }
    this->ResetTerminal();

    return 1;
}