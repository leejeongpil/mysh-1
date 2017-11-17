#include "utils.h"
#include "signal_handlers.h"
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void mysh_parse_command(const char* command,
                        int* n_commands,
                        struct single_command (*commands)[])
{
  char buf[4096];
  strcpy(buf, command);
//printf("%s\n",buf[0]);
//printf("%c\n",buf[0]);
//printf("%s\n",buf);
//printf("%c\n",buf);
//signal(SIGINT, (void *)catch_sigint);
//signal(SIGTSTP, (void *)catch_sigtstp); 
/*
  if((strncmp(buf,"/bin",4))!=0){
    strcpy(buf, "/bin/");
    strcat(buf, command);
    if((strncmp(buf,"/bin/^C",7))==0){
      memset(buf,NULL,4096);
      strcpy(buf, command);
    } else if((strncmp(buf,"/bin/^Z",7))==0){
      memset(buf,NULL,4096);
      strcpy(buf, command);
    } else if((strncmp(buf,"/bin/exit",9))==0){
      memset(buf,NULL,4096);
      strcpy(buf, command);
    }
  }
*/
  
  char *saveptr = NULL;
  char *tok = strtok_r(buf, "|", &saveptr);

  int ti = 0;

  while (tok != NULL) {
    struct single_command* com = *commands + ti;
    parse_single_command(tok, &com->argc, &com->argv);

    ++ti;

    tok = strtok_r(NULL, "|", &saveptr);
  }

  *n_commands = ti;
}

void parse_single_command(const char* command,
                          int *argc, char*** argv)
{
  const int kMaxArgc = 512;
  *argv = (char**)malloc(kMaxArgc * sizeof(char*));
  for (int i = 0; i < kMaxArgc; ++i)
    (*argv)[i] = NULL;

  char buf[4096];
  strcpy(buf, command);

  char *saveptr = NULL;
  char *tok = strtok_r(buf, " \n\t", &saveptr);

  int ti = 0;

  while (tok != NULL) {
    (*argv)[ti] = (char*)malloc(strlen(tok));
    strcpy((*argv)[ti], tok);

    ++ti;

    tok = strtok_r(NULL, " \n\t", &saveptr);
  }

  *argc = ti;

  if (*argc == 0) {
    *argc = 1;
    (*argv)[0] = (char*)malloc(1);
    (*argv)[0][0] = '\0';
  }
/*
  if((strcmp(argv[0][0],"/bin"))!=0){
    buf[0]="/bin";
    strcat(buf, argv);
    strcpy(argv, buf);
  }
*/
}
