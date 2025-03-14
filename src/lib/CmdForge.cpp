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
//  VERSION OF LIB    : 1.0.5
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
    case 0:LevelSign=FS; break;
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

    cout<<"\033[s"<<flush;
    cout<<"\033["+to_string(NewPos+1)+"G"<<flush;

    return;
}

void SysOut::Refresh(string RunSign,string CurCmd)
{
    this->UpdateInputLength((int)(RunSign.size()+CurCmd.size()));
    cout<<"\33[2K\r"<<RunSign<<CurCmd<<flush;

    return;
}

void SysOut::UpdateInputLength(int CurInputLength)
{
    s_CurInputLength=CurInputLength;

    return;
}

/////////////////////////////////////////////////////////////////////
///    CLASS : ApiCan
/////////////////////////////////////////////////////////////////////
ApiCan::ApiCan()
{
    this->Init();
}
///    PROTECTED :
/////////////////////////////////////////////////
bool ApiCan::PreCheck(vector<string> &OptsArgs)
{
    bool Flag=true;
    int i,Opts=(int)s_Opts.size();

    if (OptsArgs.size()==0) {               // CHECK: no [-opt] part.
        if (!Opts) return Flag=true;

        this->StdMsg("no [-opt] part",0);
        this->StdMsg("need help? use [-opt] '-help/-h'",0);
        Flag=false;
    }                                       // CHECK: '-help/-h' [-opt].
    else if (OptsArgs[0]=="-help"||OptsArgs[0]=="-h") {
        this->GenHelpInfo();
        Flag=false;
    }                                       // CHECK: use default [-opt].
    else if (!this->ExistOpt(OptsArgs[0])) {
        for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_D) {
            OptsArgs.insert(OptsArgs.begin(),s_Opts[0].LongFmt);
        }
        Flag=true;
    }
    return Flag;
}

bool ApiCan::PostCheck(vector<vector<string>> &OptArgs)
{
    bool Flag=true;
    string Msg;
    int i,Index,ExpArgs,ActArgs,ExpOpts=0,ActOpts=0,RepeatTimes,Size,Opts;
    unordered_multiset<string> InputOpts;

    Size=(int)OptArgs.size(),Opts=(int)s_Opts.size();
    for (i=0;i<Opts;i++) if (s_Opts[i].OptType%OPTYPE_M) ExpOpts++;
    for (i=0;i<Size;i++) InputOpts.insert(OptArgs[i][0]);

    // Check the options.
    //
    // It will check if lost mandatory options.
    // It will check if the unrepeatable option is repeat.
    if (Opts&&Size==0) {this->StdMsg("no valid [-opt].",0); return Flag=false;}

    // Find if lost mandatory options.
    for (i=0;i<Opts;i++) {
        if (s_Opts[i].OptType&OPTYPE_O) continue;

        if (!InputOpts.count(s_Opts[i].LongFmt)&&
            !InputOpts.count(s_Opts[i].ShortFmt)) {
            this->StdMsg("mandatory [-opt] '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"' is not found.",0);
            continue;
        }
        ActOpts++;
    }
    if (ActOpts<ExpOpts) {
        this->StdMsg("lost "+to_string(ExpOpts-ActOpts)+" mandatory [-opt](s).",0);
        return Flag=false;
    }
    // Find if the unrepeatable option is repeat.
    for (i=0;i<Opts;i++) {
        if (s_Opts[i].OptType&OPTYPE_R) continue;

        RepeatTimes=(int)(InputOpts.count(s_Opts[i].LongFmt)+InputOpts.count(s_Opts[i].ShortFmt));
        if (RepeatTimes>1) {
            this->StdMsg("got "+to_string(RepeatTimes)+" [-opt](s) "+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+
                         " in command, but its unrepeatable.",0);
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
            Msg="[-opt] '"+OptArgs[i][0]+"' expects " + 
                        to_string(ExpArgs)+" [-arg](s), but got " + 
                        to_string(ActArgs)+".";
            this->StdMsg(Msg,0);
            Flag=false;
        }
    }
    return Flag;
}

void ApiCan::GenHelpInfo(void)
{
    string TempMsg;
    int i,Cmds=(int)s_Cmds.size(),Opts=(int)s_Opts.size();

    TempMsg.clear();
    TempMsg=FS+"[-cmd]      =";
    for (i=0;i<Cmds;i++) TempMsg+=S+s_Cmds[i];
    this->Cout(TempMsg);
    this->Cout(FS+"  $- brief  :"+s_Brief);
    this->Cout(FS+"  $- usage  :"+s_Cmds[0]+" [-opt]/... [-arg]/...");
    if (Opts) this->Cout("");

    for (i=0;i<Opts;i++) {
        TempMsg.clear();
        TempMsg+=FS;
        
        // Generate the option type list.
        if (s_Opts[i].OptType&OPTYPE_M) TempMsg+="M";
        else TempMsg+="0";
        if (s_Opts[i].OptType&OPTYPE_D) TempMsg+="D";
        else TempMsg+=" ";
        if (s_Opts[i].OptType&OPTYPE_R) TempMsg+="R";
        else TempMsg+=" ";
        
        TempMsg+=DS+" [-opt]: "+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt;
        TempMsg+=" brief: "+s_Opts[i].Brief;
        this->Cout(TempMsg);
    }
    return;
}
///    PRIVATE :
/////////////////////////////////////////////////
void ApiCan::Init(void)
{
    s_API=nullptr;
    s_Cmds.clear();
    s_Brief.clear();

    return;
}

bool ApiCan::Check(void)
{
    bool State=true;
    
    if (!this->BasicCheck()) return State=false;
    else if (!this->OptValCheck()) return State=false;
    else if (!this->ArgValCheck()) return State=false;
    
    return State;
}

bool ApiCan::BasicCheck(void)
{
    bool State=true;
    int Opts=(int)s_Opts.size(),Cmds=(int)s_Cmds.size();

    if (s_API==nullptr) {
        this->StdMsg("no api was hooked.",2);
        return State=false;
    }
    else if (s_Cmds.size()==0) {
        this->StdMsg("no [-cmd] was hooked with this api.",2);
        return State=false;
    }
    else if (s_Opts.size()==0) {
        this->StdMsg("no [-opt] include in [-cmd] '"+s_Cmds[0]+"'.",2);
        return State=true;
    }
    return State;
}

bool ApiCan::OptValCheck(void)
{
    bool State=true;
    int i,ExpDefault=1,ActDefault=0,Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) {
        if (s_Opts[i].LongFmt.empty()&&s_Opts[i].ShortFmt.empty()) {
            this->StdMsg("you must have least one [-opt] format in [-cmd] '"+s_Cmds[0]+"'.",2);
            return State=false;
        }
    }
    for (i=0;i<Opts;i++) if (s_Opts[i].OptType&OPTYPE_D) ActDefault++;

    if (ActDefault>ExpDefault) {
        this->StdMsg("[-cmd] '"+s_Cmds[0]+"' expect only 1 default [-opt], but you set "+to_string(ActDefault)+".",2);
        return State=false;
    }
    return State;
}

bool ApiCan::ArgValCheck(void)
{
    int i,Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) {
        if (s_Opts[i].Args.size()==0) {
            this->StdMsg("you must have least 1 [-arg] in [-opt] '"+s_Opts[i].LongFmt+"/"+s_Opts[i].ShortFmt+"'.",2);
        }
    }
    return true;
}

vector<vector<string>> ApiCan::SplitOpts(vector<string> OptsArgs)
{
    vector<string> TempOptArg;
    vector<vector<string>> TempOptArgs;
    int i,j,k,Size=(int)OptsArgs.size();

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
///    PUBLIC :
/////////////////////////////////////////////////
void *ApiCan::API(void)
{
    return (void *)s_API;
}

void ApiCan::API(vector<string> CmdOptsArgs)
{
    vector<string> OptsArgs;

    // Remove [main cmd] & [-cmd] part.
    CmdOptsArgs.erase(CmdOptsArgs.begin());
    CmdOptsArgs.erase(CmdOptsArgs.begin());
    OptsArgs=CmdOptsArgs;

    // Check the original command.
    if (!this->PreCheck(OptsArgs)) return;
    s_OptArgs=this->SplitOpts(OptsArgs);

    // Check the post-process command.
    if (!this->PostCheck(s_OptArgs)) return;

    // Sort the option arguments by the option list.
    this->SortOptArgs(s_OptArgs);

    s_OptArgs.insert(s_OptArgs.begin(),s_Cmds);
    s_API(s_OptArgs);

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
    int i,Index=-1,Cmds=(int)s_Cmds.size();

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
    int i,Index=-1,Opts=(int)s_Opts.size();

    for (i=0;i<Opts;i++) {
        if (s_Opts[i].LongFmt==OptName) return Index=i;
        else if (s_Opts[i].ShortFmt==OptName) return Index=i;
    }
    return Index;
}

void ApiCan::AppendOpt(OptFmtData Opt)
{
    int Opts=(int)s_Opts.size();

    if (this->ExistOpt(Opt.LongFmt)) {
        this->StdMsg("you already have [-opt] '"+Opt.LongFmt+"' in [-cmd] '"+s_Cmds[0]+"'.",2);
        return;
    }
    else if (this->ExistOpt(Opt.ShortFmt)) {
        this->StdMsg("you already have [-opt] '"+Opt.ShortFmt+"' in [-cmd] '"+s_Cmds[0]+"'.",2);
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
///    PROTECTED :
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
    char Sign=' ';
    int i,j,Size=(int)CmdIn.size();
    vector<string> CmdSplited;

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
    int i,Index=-1,Cmds=(int)s_CmdIndex.size();

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
    int i,Index=-1,Apis=(int)s_ApiCanPool.size();

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
///    PUBLIC :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

/////////////////////////////////////////////////////////////////////
///    CLASS : FBuilder
/////////////////////////////////////////////////////////////////////

///
///    USE DEFAULT CONSTRUCTOR
///

///    PROTECTED :
/////////////////////////////////////////////////
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

bool FBuilder::CheckHooks(void)
{
    bool State=true;
    int i,Cmds=(int)s_CmdIndex.size();

    for (i=s_ResCmdNum;i<Cmds;i++) {
        if (s_ApiCanPool[s_CmdApiTable[i]].API()==nullptr) {
            this->StdMsg("you hooked [-cmd] '"+s_CmdIndex[i]+"' with a invaild <ApiCan.API() -> null>",2);
        }
    }
    if (!State) this->StdMsg("please check your command hooks.");
    
    return State;
}
///    PRIVATE :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

/////////////////////////////////////////////////////////////////////
///    CLASS : FParser
/////////////////////////////////////////////////////////////////////

///
///    USE DEFAULT CONSTRUCTOR
///

///    PROTECTED :
/////////////////////////////////////////////////
void FParser::CmdParser(string CmdIn)
{
    int Index=-1;

    this->Cout("");

    if (s_CmdIn.empty()) {                 // REJECT: empty input.
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
    }                                      // REJECT: no [-cmd] matched.
    else if ((Index=this->CmdIndex(s_CmdOptsArgs[1]))<0) { 
        this->StdMsg("invaild command, unknow [-cmd] '"+s_CmdOptsArgs[1]+"'",0);
    }
    else if (Index<s_ResCmdNum) {          // REJECT: reserved command.
        this->ForkReserved(Index);
    }
    else {
        this->ForkApi(s_CmdOptsArgs[1]);
    }
    return;
}

void FParser::SendOSCmd(string Cmd)
{
    system(Cmd.data());

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

void FParser::ForkReserved(int Index)
{
    return;
}
///    PRIVATE :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

///    PUBLIC :
/////////////////////////////////////////////////

///
///    NONE FOR THIS CLASS
///

/////////////////////////////////////////////////////////////////////
///    CLASS : ForgeHwnd
/////////////////////////////////////////////////////////////////////
ForgeHwnd::ForgeHwnd()
{
    s_CurCmdPos=0;
    s_HistoryCmd.clear();

    // Init of config
    s_Cfg.VerMode=VER_M_DEFT;
    s_Cfg.Version.clear();
    s_Cfg.ProgramName.clear();
    s_Cfg.MaxStoredCmd=10;
    s_Cfg.InputSleTime=30;
    s_Cfg.DetectSleTime=30;

    // This is the reserved command:
    // Lacking hook of api callback,you can add but do not remove them.
    this->HookApi("-help",nullptr);
    this->HookApi("-sys" ,nullptr);
    this->HookApi("-ver" ,nullptr);
    this->HookApi("-quit",nullptr);

    this->SetCmdBrief("-help","get help infomation");
    this->SetCmdBrief("-sys" ,"system interface"   );
    this->SetCmdBrief("-ver" ,"get version message");
    this->SetCmdBrief("-quit","quit the program"   );

    s_ResCmdNum=(int)s_CmdIndex.size();
}
///    PROTECTED :
/////////////////////////////////////////////////
void ForgeHwnd::CmdAutoComplete(string *CurCmd)
{
    string TempCmd;
    vector<string> TempCmds;
    int i,Cmds=(int)s_CmdIndex.size();

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

void ForgeHwnd::InputCmdTask(CmdSurfaceData *Data)
{
    size_t p;

    p=Data->CurInput.find('\n');
    if (p!=string::npos) {
        s_CmdIn.clear();
        this->SetCmdIn(Data->CurInput.substr(0,p));
        Data->CurInput.erase(0,p+1);
        Data->CursorPos=0;
    }
    if (!s_CmdIn.empty()) {
        this->StoreCmd(s_CmdIn);
        this->CmdParser(s_CmdIn);
        this->Cout(s_RunSign,0);
        s_CurCmdPos=(int)s_HistoryCmd.size();
    }
#ifdef _WIN32
    Sleep(s_Cfg.InputSleTime);
#elif __linux__
    usleep(s_Cfg.InputSleTime*1000);
#endif
    return;
}

void ForgeHwnd::DetecKeyTask(CmdSurfaceData *Data)
{
    int KeyVal=0;

    while (!Data->ExitFlag) {
        if (!_kbhit()) {
#ifdef _WIN32
            Sleep(s_Cfg.DetectSleTime);
#elif __linux__
            usleep(s_Cfg.DetectSleTime*1000);
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
                this->InputCmdTask(Data);
                break;
            case 0x1b: // Esc
                Data->ExitFlag=true;
                break;
            default:   // Character
                if (KeyVal<32||KeyVal>126) continue;
                Data->CurInput.insert(Data->CursorPos,1,KeyVal);
                Data->CursorPos++;
                break;
            }
        }
        // Update console
        this->Refresh(s_RunSign, Data->CurInput);
        this->CurMove((int)(s_RunSign.size()) + Data->CursorPos);
    }
    return;
}

bool ForgeHwnd::Check(void)
{
    bool State=true;
    int i,Apis=(int)s_ApiCanPool.size();

    if (!this->CheckHooks()) State=false;

    for (i=s_ResCmdNum;i<Apis;i++) {
        if (!s_ApiCanPool[i].Check()) return State=false;
    }
    return State;
}

///    PRIVATE :
/////////////////////////////////////////////////
void ForgeHwnd::StoreCmd(string CurCmd)
{
    if (s_HistoryCmd.size()>=s_Cfg.MaxStoredCmd) {
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

void ForgeHwnd::GenHelpInfo(void)
{
    int i,Apis=(int)s_ApiCanPool.size();
    string UsageMsg="";

    this->Cout(FS+"$- usage: "+s_MainCmd+" [-cmd] [-opt]/... [-arg]/...");

    for (i=0;i<Apis;i++) {
        this->Cout("");
        s_ApiCanPool[i].API({"","","-help"});
    }
    return;
}

void ForgeHwnd::GenVersionInfo(void)
{
    this->Cout("___________________________________________");
    this->Cout("  "+s_Cfg.ProgramName                       );
    this->Cout("  version: "+s_Cfg.Version+" "+s_Cfg.VerMode);
    this->Cout("___________________________________________");
    this->Cout("  Powered by <CmdForge>                    ");
    this->Cout("");
    
    return;
}

void ForgeHwnd::ForkReserved(int Index)
{
    // Reserved command:
    // -help : generate help message of all api.
    // -sys  : reserved system interface.
    // -ver  : generate version information of this program.
    // -quit : quit this program.
    //
    // YOU CAN ADD YOUR OWN RESERVED COMMAND HERE.
    switch (Index) {
        case 0: // -help
            this->GenHelpInfo(); break;
        case 1: // -sys
            if (s_CmdOptsArgs.size()<3) return;
            this->SendOSCmd(s_CmdOptsArgs[2]); break;
        case 2: // -ver
            this->GenVersionInfo(); break;
        case 3: // -quit
            exit(0); break;
        default:
            break;
    }
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
    switch (Mode)
    {
    case DEFAULT_M:
        s_Cfg.MaxStoredCmd=30;
        s_Cfg.InputSleTime=30;
        s_Cfg.DetectSleTime=30;
        break;
    case HIGHPFM_M:
        s_Cfg.MaxStoredCmd=10;
        s_Cfg.InputSleTime=10;
        s_Cfg.DetectSleTime=50;
        break;
    case BALANCE_M:
        s_Cfg.MaxStoredCmd=50;
        s_Cfg.InputSleTime=50;
        s_Cfg.DetectSleTime=20;
        break;
    case LOWCOST_M:
        s_Cfg.MaxStoredCmd=100;
        s_Cfg.InputSleTime=100;
        s_Cfg.DetectSleTime=10;
        break;
    }
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

void ForgeHwnd::HookCmdApi(string Cmd,void (*API)(vector<vector<string>>))
{
    this->HookApi(Cmd,API);

    return;
}

void ForgeHwnd::SetCmdBrief(string Cmd,string Brief)
{
    int Index;

    Index=this->CmdIndex(Cmd);
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].SetBrief(Brief);

    return;
}

void ForgeHwnd::SetCmdOpt(string Cmd,OptFmtData OptFmt)
{
    int Index;

    Index=this->CmdIndex(Cmd);
    Index=s_CmdApiTable[Index];
    s_ApiCanPool[Index].AppendOpt(OptFmt);

    return;
}

int ForgeHwnd::MainLoop(string RunSign)
{
    CmdSurfaceData Data;

    s_RunSign=RunSign;

    Data.CursorPos=0;
    Data.ExitFlag=false;
    Data.CurInput.clear();

    if (!this->Check()) return 0;

    this->TerminalSet();

    this->Refresh(s_RunSign,"");

    this->DetecKeyTask(&Data);

    this->TerminalReset();

    return 0;
}