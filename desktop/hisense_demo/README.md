# How to build and run demo UI application

#### 1. Install Qt framework and IDE (Qt Creator):	
	```
	sudo apt install qt5-default
	sudo apt install qtcreator
	```

#### 2. In order to build demo UI app do the following:
	1.	Navigate to the directory where yoour sources are stored
		cd <your_source_dir>

	2. 	Download sources:
		git clone https://github.com/TeeVid/sdk_samples.git

	3.	Launch Qt creator
	4.	Choose "Projects" -> "Open project" -> <your_source_dir>/sdk_samples/desktop/hisense-demo/hisense-demo.pro
	5.	Select configuration (Debug or Release)
	6.	In menu bar select:
		*	"Project" -> "Clean all"
		*	"Project" -> "Run qmake"
		*	"Project" -> "Build all" or "Rebuild all"
