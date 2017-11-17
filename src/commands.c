#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdio_ext.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "commands.h"
#include "built_in.h"
#include "utils.h"

#define SOCK_PATH  "tpf_unix_sock.server"
#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"



static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
  //{ "cat", do_cat, validate_cat_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}



/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */

void* server(void ***argv){
    char ***argv_p=(char***)argv;
//printf("server start\n");
    int server_sock, client_sock, len, rc, pid;
    int bytes_rec = 0;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;     
    char buf[256];
    int backlog = 10;
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(buf, 0, 256);                
    
    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1){
        printf("SOCKET ERROR\n");
        exit(1);
    }
    
    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
//printf("bind start\n");
    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SOCK_PATH); 
    len = sizeof(server_sockaddr);
    
    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len);
    if (rc == -1){
        printf("BIND ERROR\n");
        close(server_sock);
        exit(1);
    }
//printf("binding\n");    
    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
//printf("listen start\n");
    rc = listen(server_sock, backlog);
    if (rc == -1){ 
        printf("LISTEN ERROR\n");
        close(server_sock);
        exit(1);
    }
//printf("listening...\n");
    //printf("socket listening...\n");
    
    /*********************************/
    /* Accept an incoming connection */
    /*********************************/
//printf("accept start\n");
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock == -1){
        printf("ACCEPT ERROR\n");
        close(server_sock);
        close(client_sock);
        exit(1);
    }
//printf("accepting\n");
    
    /****************************************/
    /* Get the name of the connected socket */
    /****************************************/
/*
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (rc == -1){
        printf("GETPEERNAME ERROR: %d\n", sock_errno());
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else {
        printf("Client socket filepath: %s\n", client_sockaddr.sun_path);
    }
*/    
    /************************************/
    /* Read and print the data          */
    /* incoming on the connected socket */
    /************************************/
/*
    printf("waiting to read...\n");
    bytes_rec = recv(client_sock, buf, sizeof(buf), 0);
    if (bytes_rec == -1){
        printf("RECV ERROR: %d\n", sock_errno());
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else {
        printf("DATA RECEIVED = %s\n", buf);
    }
*/    
    /******************************************/
    /* Send data back to the connected socket */
    /******************************************/
/*
    memset(buf, 0, 256);
    strcpy(buf, DATA);      
    printf("Sending data...\n");
    rc = send(client_sock, buf, strlen(buf), 0);
    if (rc == -1) {
        printf("SEND ERROR: %d", sock_errno());
        close(server_sock);
        close(client_sock);
        exit(1);
    }   
    else {
        printf("Data sent!\n");
    }
*/    
    /******************************/
    /* Close the sockets and exit */
    /******************************/
    
    
    if((pid=fork())<0){
      perror("Can not fork process.");
    } else if(pid==0){
      close(0);
      dup2(client_sock,0);
      execv(argv_p[0][0],argv_p[0]);
      close(client_sock);
//      execv(argv_p[0],argv_p);
//      execv(com->argv[0],com->argv);
    } else{
      sleep(2); 
    }  
    close(server_sock);
    close(client_sock);
    
    //return 0;
}

void* client(void ***argv){
    char ***argv_p=(char***)argv;
//printf("client start");
    int client_sock, rc, len, pid;
    struct sockaddr_un server_sockaddr; 
    struct sockaddr_un client_sockaddr; 
    char buf[256];
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
     
    /**************************************/
    /* Create a UNIX domain stream socket */
    /**************************************/
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        printf("SOCKET ERROR(client)\n");
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    client_sockaddr.sun_family = AF_UNIX;   
    strcpy(client_sockaddr.sun_path, CLIENT_PATH); 
    len = sizeof(client_sockaddr);
    
    unlink(CLIENT_PATH);
    rc = bind(client_sock, (struct sockaddr *) &client_sockaddr, len);
    if (rc == -1){
        printf("BIND ERROR(client)\n");
        close(client_sock);
        exit(1);
    }
        
    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* for the server socket and connect   */
    /* to it.                              */
    /***************************************/
    server_sockaddr.sun_family = AF_UNIX;
    strcpy(server_sockaddr.sun_path, SERVER_PATH);
    rc = connect(client_sock, (struct sockaddr *) &server_sockaddr, len);
    if(rc == -1){
        printf("CONNECT ERROR(client)\n");
        close(client_sock);
        exit(1);
    }
    
    /************************************/
    /* Copy the data to the buffer and  */
    /* send it to the server socket.    */
    /************************************/

    /*
    strcpy(buf, DATA);                 
    printf("Sending data...\n");
    rc = send(client_sock, buf, strlen(buf), 0);
    if (rc == -1) {
        printf("SEND ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }   
    else {
        printf("Data sent!\n");
    }
    */
    /**************************************/
    /* Read the data sent from the server */
    /* and print it.                      */
    /**************************************/
    /*
    printf("Waiting to recieve data...\n");
    memset(buf, 0, sizeof(buf));
    rc = recv(client_sock, buf, sizeof(buf));
    if (rc == -1) {
        printf("RECV ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }   
    else {
        printf("DATA RECEIVED = %s\n", buf);
    }
    */
    /******************************/
    /* Close the socket and exit. */
    /******************************/
    close(1);
    dup2(client_sock,1);
    if((pid=fork())<0){
      perror("Can not fork process.");
    } else if(pid==0){
      close(client_sock);
//      execv(argv_p[0],argv_p);
      execv(argv_p[0][0],argv_p[0]);
    } else{
      sleep(2); 
    }  

    close(client_sock);
}

int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);
    int pid=0;
    int status=0;
    int trd_id;
    pthread_t p_thread[2];

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    }
    else {
      if(n_commands>=2){
	int pid,status,thr_id;
	pthread_t p_thread[2];
    	if((pid=fork())<0) perror("fork is failed!!\n");
 	else if(pid == 0){ 
          thr_id=pthread_create(&p_thread[1],NULL,&client,(void***)&(com->argv));
	  sleep(5);
	  
	} else{
  	  com=commands[0]+1;
	  thr_id=pthread_create(&p_thread[0],NULL,&server,(void***)&(com->argv));
	  sleep(5);

	}
      } else{
	if((pid=fork())<0)
	perror("fork is failed!!\n");
	else if(pid == 0){
          int exe=execv(com->argv[0],com->argv);
	  if(exe==-1){
            char temp[]="/bin/";
            com->argv[0]=strcat(temp,com->argv[0]);
            if(execv(com->argv[0],com->argv)==-1){
              char temp2[]="/usr/";
              com->argv[0]=strcat(temp2,com->argv[0]);
              execv(com->argv[0],com->argv);
            }

	    //fprintf(stderr, "%s: command not found\n", com->argv[0]);
	    return -1;
	  }
        } 
	else{
          pid=wait(&status);
        }
      }//else   
    }
  }
  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }
    free(argv);
  }
  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
