
## DEEP BREATH Breath Rate, Respiratory Volume and Chest Motion Analyzer

Created by Nili Furman

Based on previous work of Realtime Breathing by Nili Furman & Maayan Ehrenberg

Supervised by Alon Zvirin and Yaron Honen from GIP Lab in The Technion - Israel Institute of Technology

For Ichilov Hospital, Israel


### ENVIRONMENT SETUP:

1. Install Visual Studio 2019
2. Install Qt (From Qt website):

	i. Choose custom installation
	
	ii. Install Qt 5.15.1 and mcsv 2019 for this version.
	
3. Install Qt plugin for Visual Studio.
4. i. Go to the plugin:

        Extensions -> Qt VS tools -> Qt Options
    
    ii. Add version: select the mcsv2019 folder under Qt\5.15.1

### NOTES:

1. Compile only in Release mode for immediate work.
2. To compile in Debug, some changes must be made in the project's configurations (linking, include folders, etc).
3. Make sure OpenCV environment variable corresponds to the one used in the project (OPEN_CV)!
4. config.txt file must exist in the root directory AND Release directory.
