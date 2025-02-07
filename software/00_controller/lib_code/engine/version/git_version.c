



const char  git_hash_short[] =
{
  #include "git-commit-hash-short.txt"
  "\0"
};

const char  git_hash_long[] =
{
  #include "git-commit-hash-long.txt"
  "\0"
};

const char version_string[] = ("UniController-v0.1-"__DATE__" "__TIME__"\r\n");
const char version_string_git[] = ("GIT-"
#include "git-commit-hash-short.txt"
"\r\n");
