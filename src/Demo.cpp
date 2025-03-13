//  \FILE  : Demo.cpp
//  \BRIEF : A simple usage example of 'CmdForge'

#include "APIs.h"                 // Your API functions
#include "./lib/CmdForge.h"       // 'CmdForge' header file

int main() {

    // INITIALIZE YOUR 'CMD FORGE'
    ///////////////////////////////////////////////////////
    ForgeHwnd CLIF;               // CLI main interface.
    ArgFmtData ArgFmt;            // Argument format data.
    CLICfgData CLICfg;            // CLI configuration data.
    OptFmtData OptFmt;            // Option format data.
    
    // SET YOUR CLI CONFIGURATION HERE
    ///////////////////////////////////////////////////////
    CLICfg.InputSleTime=10;
    CLICfg.DetectSleTime=10;      // Suggest set in '10-100'.
    CLICfg.MaxStoredCmd=20;
    CLICfg.ProgramName="Demo";
    CLICfg.VerMode=VER_M_ALPA;
    CLICfg.Version="1.0.0.0";
    CLIF.SetCLICfg(CLICfg);
    
    // SET YOUR COMMAND HERE
    ///////////////////////////////////////////////////////
    CLIF.SetCLIMainCmd("test");
    
    // HOOK YOUR API FUNCTIONS HERE
    ///////////////////////////////////////////////////////
    CLIF.HookCmdApi("-doit",process_1);
    CLIF.HookCmdApi("-set",process_2);
    
    // Add method "-a" to command "-doit":
    CLIF.SetCmdBrief("-doit","do some thing");
    OptFmt.Optional=true;
    OptFmt.LongFmt="-bya";
    OptFmt.ShortFmt="-a";
    OptFmt.Brief="do some thing by method a";
    OptFmt.Args.push_back(ArgFmt);
    OptFmt.Args.push_back(ArgFmt);
    CLIF.SetCmdOpt("-doit",OptFmt);
    
    // [Tips] Add the different option to the same commands is allowed:
    OptFmt.Optional=true;
    OptFmt.LongFmt="-byb";
    OptFmt.ShortFmt="-b";
    OptFmt.Brief="do some thing by method b";
    CLIF.SetCmdOpt("-doit",OptFmt);
    
    OptFmt.Optional=true;
    OptFmt.LongFmt="-byc";
    OptFmt.ShortFmt="-c";
    OptFmt.Brief="do some thing by method c";
    CLIF.SetCmdOpt("-doit",OptFmt);

    // Add method "-a" to command "-set":
    CLIF.SetCmdBrief("-set","set some thing");
    OptFmt.Optional=false;
    OptFmt.LongFmt="-bya";
    OptFmt.ShortFmt="-a";
    OptFmt.Brief="set some thing by method a";
    CLIF.SetCmdOpt("-set",OptFmt);
    
    // [Warn] Add the same option to the same commands is not allowed:
    CLIF.SetCmdOpt("-set",OptFmt);

    // [Tips] Hook the same function with different commands is allowed:
    CLIF.HookCmdApi("-do",process_1);
    
    // [Warn] Hook the same command with different function is not allowed:
    CLIF.HookCmdApi("-do",process_2);
    
    // START CLI MAIN LOOP
    ///////////////////////////////////////////////////////
    CLIF.MainLoop();
    
    return 0;
}