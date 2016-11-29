@echo off

if exist output rmdir /S output
mkdir output

for /L %%i in (1,1,10) do (
copy testcase\input%%i.txt input.txt
homework.exe
copy output.txt output\output%%i.txt
)

del input.txt output.txt
