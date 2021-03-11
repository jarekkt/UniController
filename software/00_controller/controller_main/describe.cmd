:; if [ -z 0 ]; then
  @echo off
  goto :WINDOWS
fi

git rev-parse --sq HEAD  | tr "'" '"'  > git-commit-hash-long.txt
git rev-parse --sq --short HEAD | tr "'" '"'  > git-commit-hash-short.txt

# bash stuff
exit

:WINDOWS

echo Creating git info

FOR /F "delims=" %%i IN ('git rev-parse HEAD') DO set git_long=%%i

FOR /F "delims=" %%j IN ('git rev-parse --short HEAD') DO set git_short=%%j

echo Formatting git info

type %~dp0\quote.txt > git-commit-hash-long.txt
echo|set/p=%git_long% >> git-commit-hash-long.txt 
type %~dp0\quote.txt >> git-commit-hash-long.txt

type %~dp0\quote.txt > git-commit-hash-short.txt
echo|set/p=%git_short% >> git-commit-hash-short.txt
type %~dp0\quote.txt >> git-commit-hash-short.txt


:: windows stuff


