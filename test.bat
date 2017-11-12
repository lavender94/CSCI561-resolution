@echo off

set PATH_TESTCASE=testcase\testcase\testcase\
set TESTCASE_NUMBER=41

if exist output rmdir /FS output
mkdir output

for /L %%i in (1,1,%TESTCASE_NUMBER%) do (
copy %PATH_TESTCASE%input%%i.txt input.txt
homework.exe
copy output.txt output\output%%i.txt
diff -q -Z -b -I --suppress-common-lines output\output%%i.txt %PATH_TESTCASE%output%%i.txt
)

del input.txt output.txt
