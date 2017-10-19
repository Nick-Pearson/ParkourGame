startingdir=$(pwd)

if [ -z "$UNREAL_PATH" ]; then
  echo "ERROR : Environment var UNREAL_PATH must be set"
  exit 1
fi

# find out which git commit we are on
githash=$(git rev-parse HEAD)

if [ -z "$githash" ]; then
	githash="local_build"
else
	githash=${githash:0:7}
fi

#echo $githash

#create output directories for each build
cd ..
projdir=$(pwd)

if ! [ -d ./Builds ]; then
	mkdir Builds
fi

cd Builds

if [ -d ./$githash ]; then
	echo "A build for $githash already exists, would you like to overwrite it? [Y/n]"
	read delBuild

	if [ ${delBuild:0:1} = "Y" ]; then
		rm -r ./$githash
	else
		echo "Build Aborted : existing build for this version in directory"
		exit 1
	fi
fi

mkdir $githash
cd $githash

mkdir Client
mkdir Server

cd ../../Scripts

# CLIENT
echo ""
echo "Building Client"

set -x
"$UNREAL_PATH/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun -project="$projdir/ParkourGame/ParkourGame.uproject" -noP4 -platform=Linux -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="$projdir/Builds/$githash/Client" > Buildlog_${githash}_Client.txt
set +x

# check binary exists
if [ -e "$projdir\Builds\$githash%\Client\WindowsNoEditor\ParkourGame.out" ]; then
	echo ""
	echo "*** Client built successfully ***"
else
	echo ""
	echo "*** Client built with errors ***"
	exit 1
fi

# SERVER
echo ""
echo "Building Server"

set -x
"$UNREAL_PATH/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun -project="$projdir/ParkourGame/ParkourGame.uproject" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -server -serverplatform=Win64 -noclient -build -stage -pak -archive -archivedirectory="$projdir/Builds/$githash/Server" > Buildlog_${githash}_Server.txt
set +x

# check binary exists
if [ -e "$projdir/Builds/$githash/Server/LinuxNoEditor/ParkourGameServer.out" ]; then
	echo ""
	echo "*** Server built successfully ***"
else
	echo ""
	echo "*** Server built with errors ***"
	echo "NOTE: Servers must be built from Github Source"
	exit 1
fi

cd $startingdir
