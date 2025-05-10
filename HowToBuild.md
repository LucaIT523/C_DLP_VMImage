1. Requirements

	- Preinstall
		> Visual Studio 2022(Desktop, C, MFC)
		> Node (more than v18)
		
	- Create Environment
		> open command prompt on win\launcher\node\kclient and run
		# npm install -g pkg
		# npm install
		
		> open command prompt on win\launcher\node\kagent and run
		# npm install -g pkg
		# npm install
		
2. Build

	- Launcher App
	  launch Visual Studio and open solution(win\launcher\launcher.sln)
	  # set configuration to release & x64 and rebuild all
	  # set configuration to release & x86 and rebuild all

	- kclient
	  # open command prompt on win\launcher\node\kclient and run
	  # pkg .
	  
	- kagent
	  # open command prompt on win\launcher\node\kagent and run
	  # pkg .

3. Deploy

	- run zRelease.bat
	
4. Build Result

	Build result will be deploied to win\_bin\release