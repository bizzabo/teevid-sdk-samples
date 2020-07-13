# How to build and run demo UI application

#### 1. Install Qt framework and IDE (Qt Creator):	
```
sudo apt install qt5-default
sudo apt install qtcreator
```

#### 2. In order to build demo UI app do the following:
	1.	Navigate to the directory where your sources are stored
		cd <your_source_dir>

	2. 	Download sources:
	        cd cd teevid-client-native
		git clone https://github.com/TeeVid/sdk_samples.git

	3.	Run script to fix library issues:
		./install.sh

	4.	Launch Qt creator
	5.	Choose "Projects" -> "Open project" -> <your_source_dir>/sdk_samples/desktop/teevid-client-native/teevid-client-native.pro
	6.	Select configuration (Debug or Release)
	7.	In menu bar select:
		*	"Project" -> "Clean all"
		*	"Project" -> "Run qmake"
		*	"Project" -> "Build all" or "Rebuild all"
