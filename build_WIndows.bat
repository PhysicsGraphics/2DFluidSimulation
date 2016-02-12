@echo off

mkdir outWindows
cd outWindows

cmake ../src/ -G "Visual Studio 12 2013"

cd ..