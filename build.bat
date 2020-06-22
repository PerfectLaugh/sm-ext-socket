set EXT_DIR="%cd%"
set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"

if "%VSCMD_VER%"=="" (
	set MAKE=
	set CC=
	set CXX=
	call %VCVARSALL% x86
)

git clone https://github.com/alliedmodders/sourcemod --recursive --branch "%BRANCH%" --single-branch "%EXT_DIR%/sourcemod-%BRANCH%"

; Boost
wget https://dl.bintray.com/boostorg/release/1.73.0/source/boost_1_73_0.7z
7z x boost_1_73_0.7z > nul
7z x boost_stage.7z -o boost_1_73_0

mkdir "%EXT_DIR%/build"
pushd "%EXT_DIR%/build"
python "%EXT_DIR%/configure.py" --enable-optimize --sm-path "%EXT_DIR%/sourcemod-%BRANCH%" || goto error
ambuild || goto error
popd

:error
exit /b %errorlevel%