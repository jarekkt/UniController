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
git rev-parse --sq HEAD > %~dp0\git-commit-hash-long.txt
git rev-parse --sq --short HEAD  > %~dp0\git-commit-hash-short.txt

%~dp0\sed.exe -i s/\x27/\"/g %~dp0\git-commit-hash-long.txt
%~dp0\sed.exe -i s/\x27/\"/g %~dp0\git-commit-hash-short.txt
del  %~dp0\sed*. >nul 2>&1
del  sed*.  >nul 2>&1

:: windows stuff


