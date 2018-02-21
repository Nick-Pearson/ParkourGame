# Parkour Project

Physics-Driven Multiplayer Parkour Game

## Installation

1. Clone the repository

``` bash
$ git clone https://github.com/Nick-Pearson/ParkourGame.git
```

**NOTE:** As unreal uses many binary files we are using git Large File Storage to handle these, if you cannot sync properly check you have git LFS installed and working - on windows there is
a known issue where you will need a credential manager to sync properly (git credential cache is an option for this)

### IDE Workflow

IDE Workflow runs the whole editor out of an IDE

2. Navigate to /ParkourGame/, and right click on ParkourGame.uproject. You should see an option to "Generate project files" for Visual Studio or XCode

3. Open the solution file generated and build, once a build is sucessful the Unreal Editor will appear

### Editor Workflow

You can run the editor without IDE, may be useful for smaller changes or on low powered machines

2. Launch Unreal Engine (4.18) from the Epic Games Launcher

3. In the project selector browse to the /ParkourGame/ folder and select ParkourGame.uproject

4. When the editor appears you will be able to run like normal, you can edit code and use the Editor's built in hot-reload compiler to make changes (this can be unstable and often a full editor reload is required)

5. (optional) If you later open an IDE with the game solution you can use the Attach to Process tool to link the debugger directly to the editor

## Link Unreal to Git

Unreal can show version control information in the Editor

1. Click 'Source Control' from the main toolbar and select 'Connect to source control'

2. Select 'git (beta version)' from the provider dropdown, everything should auto populate. If not you may have to navigate manually to your git binary

