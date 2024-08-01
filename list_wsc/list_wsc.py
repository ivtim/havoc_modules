from havoc import Demon, RegisterCommand, RegisterModule
from os.path import exists


def list_wsc( demonID, *param ):
    TaskID : str    = None
    demon  : Demon  = None
    packer : Packer = Packer()

    demon = Demon( demonID )
    
    if len(param) < 1:
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "Not enough arguments" )
        return False
    
    type_wsc = param[0]
    param_code = 0
    if 'av' in type_wsc:
        param_code = 0x4
    elif 'as' in type_wsc:
        param_code = 0x8
    elif 'fr' in type_wsc:
        param_code = 0x1
    else:
        demon.ConsoleWrite( demon.CONSOLE_ERROR, "Wrong first argument" )
        return False

    TaskID = demon.ConsoleWrite( demon.CONSOLE_TASK, f"Tasked demon to enumiate wsc" )

    packer.addint(param_code)
    demon.InlineExecute( TaskID, "go", f"list_wsc.{demon.ProcessArch}.o", packer.getbuffer(), False )

    return TaskID


RegisterCommand( list_wsc, "", "list_wsc", "enumirate wsc sessions", 0, "[SECURITY_PROVIDER] (ANTIVIRUS | ANTISPYWARE | FIREWALL)", "av | as | fr" )
