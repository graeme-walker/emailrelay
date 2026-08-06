# fix/service-wrapper-cwd

If the startup batch file contains "start dir/emailrelay.exe" then
the service has to cd to the batch file's directory before doing
CreateProcess("dir/emailrelay.exe").
