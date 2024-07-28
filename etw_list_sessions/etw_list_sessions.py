from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists


def etw_list_sessions( demonID, *param ):
    TaskID : str    = None
    demon  : Demon  = None

    demon = Demon( demonID )

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Tasked demon to enumiate etw sessions" )

    demon.InlineExecute( TaskID, "go", f"etw_list_sessions.{demon.ProcessArch}.o", b'', False )

    return TaskID


RegisterCommand( etw_list_sessions, "", "etw_list_sessions", "enumirate etw sessions", 0, "", "" )
