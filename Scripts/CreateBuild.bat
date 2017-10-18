@echo off

set startingdir=%cd%

if  "%UNREAL_PATH%"=="" (
	echo ERROR : Environment var UNREAL_PATH must be set
	goto done
)

:: find out which git commit we are on
set githash=
git rev-parse HEAD > hash.txt
set /p githash=<hash.txt
del hash.txt

if "%githash%"=="" (
	set githash=local_build
) else (
	set githash=%githash:~0,7%
)

::echo %githash%

::create output directories for each build
cd ..
set projdir=%cd%

if not exist Builds (
	mkdir Builds
)

cd Builds

if exist %githash% (
	echo A build for %githash% already exists, would you like to overwrite it?
	set delBuild=
	set /p delBuild=[Y/n]?:

	if "%delBuild:~0,1%"=="Y" (
		rmdir /q /s %githash%
	) else (
		echo Build Aborted : existing build for this version in directory
		goto done
	)
)
mkdir %githash%
cd %githash%

mkdir Client
mkdir Server

cd ..\..\Scripts

:: CLIENT
echo.
echo Building Client

@echo on
call "%UNREAL_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%projdir%\ParkourGame\ParkourGame.uproject" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="%projdir%\Builds\%githash%\Client" > Buildlog_%githash%_Client.txt
@echo off

:: check binary exists
if not exist %projdir%\Builds\%githash%\Client\WindowsNoEditor\ParkourGame.exe (
	echo.
	echo *** Client built with errors ***
	goto done
) else (
	echo.
	echo *** Client built successfully ***
)

:: SERVER
echo.
echo Building Server

@echo on
call "%UNREAL_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project="%projdir%\ParkourGame\ParkourGame.uproject" -noP4 -platform=Win64 -clientconfig=Development -serverconfig=Development -cook -server -serverplatform=Win64 -noclient -build -stage -pak -archive -archivedirectory="%projdir%\Builds\%githash%\Server" > Buildlog_%githash%_Server.txt
@echo off

:: check binary exists
if not exist %projdir%\Builds\%githash%\Server\WindowsNoEditor\ParkourGameServer.exe (
	echo.
	echo *** Server built with errors ***
	echo NOTE: Servers must be built from Github Source
	goto done
) else (
	echo.
	echo *** Server built successfully ***
)

:done
	cd %startingdir%
	set startingdir=
	set githash=
	set projdir=
