@echo off

cd ..
mkdir _bin
mkdir _bin\release
mkdir _bin\release
mkdir _bin\release\launcher
mkdir _bin\release\agent

copy .\_release\klauncher32.exe 			_bin\release\launcher\
copy .\_release\launcher64.exe 				_bin\release\launcher\
copy .\_release\KenanConn.dll  				_bin\release\launcher\
copy .\_release\KenanHelperVM32.dll			_bin\release\launcher\
copy .\_release\KenanHelperVM64.dll			_bin\release\launcher\
copy .\launcher\cmn_vm17\*.dll 				_bin\release\launcher\
copy .\launcher\ext_bin\curl\* 				_bin\release\launcher\
copy .\launcher\ext_bin\router\* 			_bin\release\launcher\
copy .\launcher\ext_bin\qrdecode\* 			_bin\release\launcher\
copy .\launcher\ext_bin\7z\* 				_bin\release\launcher\
mkdir _bin\release\launcher\certs
copy .\launcher\ext_bin\router\certs\* 		_bin\release\launcher\certs\
copy .\launcher\script\config.ini   		_bin\release\launcher\
copy .\_release\KUpdateApp.exe 				_bin\release\launcher\

copy .\_release\agent_svc.exe 				_bin\release\agent\
copy .\_release\kagentqr.exe 				_bin\release\agent\
copy .\_release\kenanagent.exe 				_bin\release\agent\
copy .\launcher\ext_bin\poco\release\* 		_bin\release\agent\
copy .\launcher\script\regservice.bat   	_bin\release\agent\
copy .\launcher\ext_bin\7z\* 				_bin\release\agent\
copy .\launcher\ext_bin\agent\* 			_bin\release\agent\
mkdir _bin\release\agent\certs
copy .\launcher\ext_bin\agent\certs\* 		_bin\release\agent\certs\
copy .\_release\KUpdateApp.exe 				_bin\release\agent\

pause