from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists


def enum_logical_drive( demonID, *param ):
    TaskID : str    = None
    demon  : Demon  = None

    demon = Demon( demonID )

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Tasked demon to enumiate logical driver" )

    demon.InlineExecute( TaskID, "go", f"enum_logical_drive.{demon.ProcessArch}.o", b'', False )

    return TaskID


RegisterCommand( enum_logical_drive, "", "enum_logical_drive", "enumirate logical drive on machine", 0, "", "" )
