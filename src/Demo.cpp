//  \FILE  : Demo.cpp
//  \BRIEF : A simple usage test example of 'CmdForge'
// ----------------------------------------------------------------------------
//  [QUICK START]
//
//      1. Copy or move folder './lib' to your project folder.
//      2. Create a new file (such as './API.h'), Include your project and
//         then write your API functions at this file.
//      3. Create a new file (such as './Demo.cpp'), Include './lib/CmdForge.h'
//         and './API.h', and then 'forge' your 'CmdForge'.
//      4. Compile and run your command line interface program.
//      5. Free to use!
//
//      For more details, please see the 'README.md' or files in the '.doc/'.
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

int main(int argc,char *argv[])
{
    // INITIALIZE YOUR 'CmdForge'
    ///////////////////////////////////////////////////////
    ForgeHwnd CLIF(argc,argv);    // CLI main interface.
                                  // (Only interact mode when use default).
    ArgFmtData ArgFmt={0};        // Argument format data.
    CLICfgData CLICfg={0};        // CLI configuration data.
    OptFmtData OptFmt={0};        // Option format data.

    // SET YOUR CLI CONFIGURATION HERE
    ///////////////////////////////////////////////////////
    CLICfg.InputSleepTime=20;
    CLICfg.DetectSleepTime=20;    // Suggest set in '10-100'.
    CLICfg.MaxStoredCmd=20;
    CLICfg.ProgramName="Demo Process";
    CLICfg.Version="1.0.0.0";
    CLIF.SetCLICfg(CLICfg);

    // SET YOUR MAIN COMMAND AND SUB-COMMANDS HERE
    ///////////////////////////////////////////////////////
    CLIF.SetCLIMainCmd("proc");

    CLIF.HookCmdApi("-start",process_1);
    CLIF.SetCmdBrief("-start","start main process");

    CLIF.HookCmdApi("-do",process_2);
    CLIF.SetCmdBrief("-do","do process");

    CLIF.HookCmdApi("-stop",process_3);
    CLIF.SetCmdBrief("-stop","stop process");

    CLIF.HookCmdApi("-end",process_4);
    CLIF.SetCmdBrief("-end","end main process");

    OptFmt.LongFmt="--id";
    OptFmt.ShortFmt="-i";
    OptFmt.Brief="process id";
    OptFmt.Args.push_back(ArgFmt);
    OptFmt.OptType=OPTYPE_D|OPTYPE_M;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

    OptFmt.LongFmt="--task";
    OptFmt.ShortFmt="-t";
    OptFmt.Brief="process task";
    OptFmt.OptType=OPTYPE_M|OPTYPE_R;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

    OptFmt.LongFmt="--occupy";
    OptFmt.ShortFmt="-o";
    OptFmt.Brief="process occupy";
    OptFmt.OptType=OPTYPE_O;
    CLIF.SetCmdOpt("-do",OptFmt);
    CLIF.SetCmdOpt("-stop",OptFmt);

    // THESE ARE TESTS YOU CAN DEBUG TO KNOW MORE DETAIL
    ///////////////////////////////////////////////////////
#ifdef _TEST0
    // This test show a few warnings about subcommand hooking:
    //
    //     1. A subcommand can only hook with one package 
    //        function.
    //     2. Links to duplicate subcommands and package 
    //        functions are automatically ignored by CmdForge.
    //     3. CmdForge checks that the hooked package function 
    //        if is valid (Do not include reserved subcommands).
    //     4. Cause by the new hook introduced in 1.0.7, there
    //        are some new warnings.
    //

    // 1.
    CLIF.HookCmdApi("-do",process_2);

    // 2.
    CLIF.HookCmdApi("-do",process_1);

    // 3.
    CLIF.HookCmdApi("-chk",nullptr);

    // 4.
    CLIF.HookCmdApi("-chk","-do");
    CLIF.HookCmdApi("-do","-do");
    CLIF.HookCmdApi("-do","-stop");

#endif

#ifdef _TEST1
    // This test show a few tips about subcommand hooking:
    //
    //     1. You can hook different subcommand with a package
    //        function, in other words, different commands can
    //        share the same package function.
    //     2. You can hook new command to a exist command
    //        (new introduced in 1.0.7).
    //
    // So, we can use command like:
    //
    //      { proc -do -i 01 -task swap }      or
    //      { proc --doit -i 01 -task swap }   or
    //      { proc --dothis -i 01 -task swap }
    // 
    // to call the same function.
    //

    // 1.
    CLIF.HookCmdApi("--startit",process_1);
    CLIF.HookCmdApi("--doit",process_2);
    CLIF.HookCmdApi("--stopit",process_3);
    CLIF.HookCmdApi("--endit",process_4);

    // 2.
    CLIF.HookCmdApi("-do","-dothis");
    CLIF.HookCmdApi("-stop","-stopthis");

#endif
    // START CLI MAIN LOOP
    ///////////////////////////////////////////////////////
    CLIF.MainLoop();

    return 0;
}
