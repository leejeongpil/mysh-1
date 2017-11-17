#include "signal_handlers.h"
#include <signal.h>
#include <stdio.h>
//#include "command.c"
void catch_sigint(int signalNo)
{
  //fflush(stdout);
  //getchar();
  //while(getchar()!='\n');
  printf("\n# your input Ctrl+C\n# Shell doesn't close.\n");
  signal(SIGINT,SIG_IGN);
  // TODO: File this!
}

void catch_sigtstp(int signalNo)
{
  //fflush(stdout);
  //getchar();
  //while(getchar()!='\n');
  printf("\n# your input Ctrl+Z, but it doesn't move.\n");
  signal(SIGTSTP,SIG_IGN);
  // TODO: File this!
}
