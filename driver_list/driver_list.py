from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists


def driver_list( demonID, *param ):
    TaskID : str    = None
    demon  : Demon  = None

    demon = Demon( demonID )

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Tasked demon to enumiate drivers" )

    demon.InlineExecute( TaskID, "go", f"driver_list.{demon.ProcessArch}.o", b'', False )

    return TaskID


RegisterCommand( driver_list, "", "driver_list", "enumirate drivers on machine", 0, "", "" )
