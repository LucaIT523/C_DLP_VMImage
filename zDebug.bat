@echo off

cd ..
mkdir _bin
mkdir _bin\debug
mkdir _bin\debug
mkdir _bin\debug\launcher
mkdir _bin\debug\agent

copy .\_debug\launcher32.exe 				_bin\debug\launcher\
copy .\_debug\launcher64.exe 				_bin\debug\launcher\
copy .\_debug\KenanConn.dll  				_bin\debug\launcher\
copy .\_debug\KenanHelperVM32.dll			_bin\debug\launcher\
copy .\_debug\KenanHelperVM64.dll			_bin\debug\launcher\
copy .\launcher\cmn_vm17\*.dll 				_bin\debug\launcher\
copy .\launcher\ext_bin\curl\* 				_bin\debug\launcher\
copy .\launcher\ext_bin\router\* 			_bin\debug\launcher\
mkdir _bin\debug\launcher\certs
copy .\launcher\ext_bin\router\certs\* 		_bin\debug\launcher\certs\
copy .\launcher\script\config.ini   		_bin\debug\launcher\

copy .\_debug\agent_svc.exe 				_bin\debug\agent\
copy .\_debug\kenanagent.exe 				_bin\debug\agent\
copy .\launcher\ext_bin\poco\debug\* 		_bin\debug\agent\
copy .\launcher\script\regservice.bat   	_bin\debug\agent\
copy .\launcher\ext_bin\agent\* 			_bin\debug\agent\
mkdir _bin\debug\agent\certs
copy .\launcher\ext_bin\agent\certs\* 		_bin\debug\agent\certs\

pause