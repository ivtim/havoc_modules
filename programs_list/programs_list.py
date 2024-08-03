from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists


def programs_list( demonID, *param ):
    TaskID : str    = None
    demon  : Demon  = None

    demon = Demon( demonID )

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Tasked demon to enumirate installed programs" )

    demon.InlineExecute( TaskID, "go", f"programs_list.{demon.ProcessArch}.o", b'', False )

    return TaskID


RegisterCommand( programs_list, "", "programs_list", "enumiarte installed programs", 0, "", "" )
