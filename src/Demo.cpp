//  \FILE  : Demo.cpp
//  \BRIEF : A simple usage test example of 'CmdForge'
// ----------------------------------------------------------------------------
//  [INSTRUCTION]
//  
//  The content of these test example is suitable for commond use cases:
//  
//      1. Generally have no more than 10 subcommands
//         (include reserved subcommands).
//      2. A subcommand has no more than 3 options
//         (the type of options can be arbitrary).
//  
//  Define 4 subcommands. 
//      '-start' '-do' '-stop' '-end'.
//  
//  '-do' and '-stop' have 3 options. '-a' '-b' '-c'
//  
//  So, we have two basic types of a command:
//  
//  1. Without options (like: '-start' '-end').
//  
//      {$ MainCmd} {$ -SubCmd}
//  
//      exp. test -start
//  
//  2. With options (like: '-do' '-stop').
//  
//      {$ MainCmd} {$ -SubCmd} [ {$ -Opt} [{$ -Arg}](s) ](s)
//  
//      exp. test -do -a run -b swim -c read
//  
//  [ATTENTION]
//  
//      CmdForge will always auto-check the valid. So you might get some 
//      infomations, warnings, and errors after build.
//  
//      CmdForge will stop when you have at least one error even fatal. Other 
//      infomations and warnings will be omitted.
//      
//      But we also strongly suggest you to check every warnings, because this 
//      only show CmdForge don't meet that, prevent the running of main loop, 
//      and not actually know what you want to do.
// ----------------------------------------------------------------------------

#include "APIs.h"                 // Your API functions
#include "./lib/CmdForge.h"       // 'CmdForge' header file

int main(int args,char *argv[]) 
{
    // INITIALIZE YOUR 'CmdForge'
    ///////////////////////////////////////////////////////
    ForgeHwnd CLIF;               // CLI main interface.
    ArgFmtData ArgFmt={0};        // Argument format data.
    CLICfgData CLICfg={0};        // CLI configuration data.
    OptFmtData OptFmt={0};        // Option format data.
    
    // SET YOUR CLI CONFIGURATION HERE
    ///////////////////////////////////////////////////////
    CLICfg.InputSleTime=20;
    CLICfg.DetectSleTime=20;      // Suggest set in '10-100'.
    CLICfg.MaxStoredCmd=20;
    CLICfg.ProgramName="Demo";
    CLICfg.VerMode=VER_M_ALPA;
    CLICfg.Version="1.0.0.0";
    CLIF.SetCLICfg(CLICfg);
    
    // SET YOUR MAIN COMMAND HERE
    ///////////////////////////////////////////////////////
    CLIF.SetCLIMainCmd("test");

    // THESE ARE TESTS YOU CAN DEBUG TO KNOW MORE DETAIL
    ///////////////////////////////////////////////////////
    CLIF.HookCmdApi("-start",process_1);
    CLIF.SetCmdBrief("-start","start main");

    CLIF.HookCmdApi("-do",process_2);
    CLIF.SetCmdBrief("-do","do some thing");

    CLIF.HookCmdApi("-stop",process_3);
    CLIF.SetCmdBrief("-stop","stop some thing");

    CLIF.HookCmdApi("-end",process_4);
    CLIF.SetCmdBrief("-end","end main");

    OptFmt.LongFmt="--along";
    OptFmt.ShortFmt="-a";
    OptFmt.Brief="option along/a";
    OptFmt.Args.push_back(ArgFmt);
    OptFmt.OptType=OPTYPE_D|OPTYPE_O;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

    OptFmt.LongFmt="--blong";
    OptFmt.ShortFmt="-b";
    OptFmt.Brief="option blong/b";
    OptFmt.OptType=OPTYPE_M|OPTYPE_R;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

    OptFmt.LongFmt="--clong";
    OptFmt.ShortFmt="-c";
    OptFmt.Brief="option blong/b";
    OptFmt.OptType=OPTYPE_M;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

#ifdef _TEST0
    // This test illustrates a few warns about subcommand hooking:
    //
    //     1. A subcommand can link only one package 
    //        function.
    //     2. Links to duplicate subcommands and package 
    //        functions are automatically ignored by CmdForge.
    //     3. CmdForge checks that the linked package 
    //        function is valid.
    //

    // 1.
    CLIF.HookCmdApi("-do",process_2);

    // 2.
    CLIF.HookCmdApi("-do",process_1);

    // 3.
    CLIF.HookCmdApi("-chk",nullptr);

#endif

#ifdef _TEST1
    // This test illustrates a few tips about subcommand hooking:
    //
    //     1. You can hook different subcommand with a package
    //        function, in other words, different commands can
    //        share the same package function.
    //
    // So, we can use command like:
    //
    //      { test -do -a read } or { test -doit -a read }
    // 
    // to call the same function.
    //
    
    // 1.
    CLIF.HookCmdApi("-startit",process_1);
    CLIF.HookCmdApi("-stopit",process_2);
    CLIF.HookCmdApi("-doit",process_3);
    CLIF.HookCmdApi("-endit",process_4);

#endif
    
    // START CLI MAIN LOOP
    ///////////////////////////////////////////////////////
    CLIF.MainLoop();
    
    return 0;
}