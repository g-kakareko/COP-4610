/**************************************
	
	Authors: 
	1)	Grzegorz Kakareko
	email: gk15b@my.fsu.edu
	2)	Jordan Camejo
	email: 
	3)	Grace Bunch
	email: gib14@my.fsu.edu

**************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/wait.h>

#define MAXCHAR 255  // Max length of a single line of code.

void my_setup();
void my_prompt();
char **my_parse (char *line);
int my_execute (char ** cmd);
void my_clean ();
char *my_read ();
char *parse_whitespace(char *line); 
char **parse_arguments(char *line);
char **resolve_paths(char **args);
char *convert_path(char *args);
char **expand_variables(char **args);
char *space_special_char(char *line, char speacial_char);
void builtins(char **cmd);


int arg_size;
int main()
{
            
  char *line;
	char **cmd;
  while (1) 
  {
    my_setup();
		my_prompt();
		line = my_read();
		cmd = my_parse(line);
		builtins(cmd);
		my_execute(cmd);
		//my_clean();
	       
		//Setup
		//Print prompt
		//Read input
		//Transform input
		//Match against patterns
		//Execute command
		//Print results
		//Cleanup
	}
    return 0;
}

void my_setup() {}
void my_prompt() 
{
	//get username
  //printf(getenv("USER"));
  printf("%s",getenv("USER"));
  printf("%s","@");
 
  //get hostname
  char hostBuffer[256];
  int hostname;
  hostname=gethostname(hostBuffer, sizeof(hostBuffer));
  printf("%s",hostBuffer);

  printf(" :: ");
  //get absolute directory
  char cwd[1024];
  if(getcwd(cwd, sizeof(cwd))!= NULL)
    printf("%s",cwd);
  printf(" => ");
}


char **my_parse (char *line) 
{	
	// char *toParseStr = (char*)malloc(10);
	// printf("Enter string here: \n");
	// scanf("%s",toParseStr);
	// printf("%s\n",toParseStr);
	// free(toParseStr);
	// return NULL;
	char **args;
	char *clear_line;
	line = parse_whitespace(line);
	// line=clear_line;
	// printf("clear line: %s\n", line);
	args = parse_arguments(line);
	int i;
	// for (i = 0; i < arg_size; ++i) 
  //       printf("%s\n", args[i]);
	// args = expand_variables(args);

	// path resolution - grace
	args = resolve_paths(args);
  for (i = 0; i < arg_size; ++i) 
        printf("%s\n", args[i]);


	return args;
}
char *parse_whitespace(char *line) 
{
/**************************************
	Removing extra white spaces
***************************************/
	int i,x;
  	for(i=x=0; line[i]; ++i)
    	if(!isspace(line[i]) || (i>0 && !isspace(line[i-1])))
      		line[x++] = line[i];
  	line[x] = '\0';
/**************************************
	Adding extra white spaces for special characters
	Special characters include: |, <, >, &, $, ~
***************************************/
	line = space_special_char(line, '<');
	line = space_special_char(line, '>');
	line = space_special_char(line, '|');
	line = space_special_char(line, '&');
	//line = space_special_char(line, '$');
	//printf("Final line: %s\n", line);

	return line;
}

char *space_special_char(char *line, char speacial_char)
{
	int i;
	//for(i=0; i < strlen(line); i++)
  	for(i=0; line[i] != '\0'; i++)
  	{
    // if(line[i]=='|' || line[i] == '<' || line[i] == '>' || line[i]== '&' ||
    //    line[i]=='$' || line[i] == '~')
      if(line[i]==speacial_char)
      {
      	bool t1; // [i-1]
      	bool t2; // [i+1]
      	bool t3; // [i+1] and [i+1]
      	t1=line[i-1]!=' ';
      	t2=line[i+1]!=' ';
      	if(t1 && t2)
      	{
			char *new_line;
			new_line=(char*) malloc(MAXCHAR);

			int k;
			for(k=0; k < (i); k++)
			{
				new_line[k]=line[k];
			}
			new_line[i]=' ';
			new_line[i+1]=line[i];
			new_line[i+2]=' ';
			int j;
			//printf("%d\n", k);
			for(j=k; j < (strlen(line)); j++)
			{
				new_line[j+3]=line[j+1];
			}
			//line=new_line;
			new_line[j+3]='\0';
			line=new_line;
			//printf("%s\n","t1 && t2" );
			//printf("new_line: %s\n", new_line);	

      	}else if(t1)
      	{
      		char *new_line;
			new_line=(char*) malloc(MAXCHAR);

			int k;
			for(k=0; k < (i); k++)
			{
				new_line[k]=line[k];
			}
			new_line[i]=' ';
			new_line[i+1]=line[i];
			int j;
			for(j=k; j < (strlen(line)); j++)
			{
				new_line[j+2]=line[j+1];
			}
			new_line[j+2]='\0';
			line=new_line;
			//printf("%s\n","t1" );
			//printf("new_line: %s\n", new_line);

      	}else if(t2)
      	{
      		char *new_line;
			new_line=(char*) malloc(MAXCHAR);

			int k;
			for(k=0; k <= (i); k++)
			{
				new_line[k]=line[k];
			}
			new_line[i+1]=' ';
			int j;
			for(j=k; j < (strlen(line)); j++)
			{
				new_line[j+1]=line[j];
			}
			new_line[j+1]='\0';
			line=new_line;
			//printf("%s\n","t2" );
			//printf("new_line: %s\n", new_line);

      	}
      	}
      }
  	//printf("line: %s\n", line);
	return line;	
} 

char **parse_arguments(char *line)
{
	int i;
	int k=0;
	//for(i=0; line[i] != '\0'; i++)
	int d =strlen(line);
	//printf("Size: %d\n", d);
	//printf("Line in parse_arguments: %s\n", line);
  	for(i=0; i <strlen(line); i++)
  	{
  		//if((line[i]=' ') && (i!=0))
  		if(isspace(line[i]) && (i!=0))
  		{	
  			//printf("%s\n","We have space" );
  			if(!isspace(line[i-1]))
  			{
  				//printf("%s\n","We have space" );
  				k++;
  			}
  		}
  	}	
  	arg_size=k;
  	//printf("spaces: %d\n", k);
	char **vector_arg;
	vector_arg=(char**) malloc((k)*MAXCHAR);

//---------------------------
	//printf( "Tokens:\n" );
	char seps[] = " ";
	char *token;
    /* Establish string and get the first token: */
    token = strtok(line, seps );
    int j =0;
    vector_arg[j]=token;
    j++;
    while( token != NULL )
    {
        /* While there are tokens in "string" */
        //printf( " %s\n", token );
        /* Get next token: */
        token = strtok( NULL, seps );
	vector_arg[j]=token;
		j++;        
    }
    // for (i = 0; i < k; ++i) 
    //     printf("%s\n", vector_arg[i]);
    

    

	//return NULL;
	return vector_arg;
}

char **resolve_paths(char **cmd)
{
	//code should be fixed now - grace
	//
 char *temp;
  int i, j;
  //get path 
  char *path = getenv("PATH");
  path[strlen(path)]='\0';
  char split_path[100][100];
   int exit=0;
  j=0;
  int l=0;


  //set split_path
  for(i=0; path[l]!='\0'; i++){
    
    while(exit!=1 && path[l]!='\0'){

      if(path[l]==':'){
        l++;
        exit=1;
        split_path[i][j]='\0';
       }
      else{
        split_path[i][j]=path[l];
        j++;
        l++;

      }
    }
    exit=0;
    j=0;

  }

  int k;
  int count=0;
 
  for(i=0; i <100; i++){
    if(cmd[i]!="\0" && cmd[i]!=" " && cmd[i]!="\n" && cmd[i]!=NULL){
     
      count++;
    }
  }
 

  char **tempArr=cmd;
 
  for(k=0; k<count-1;k++){

    //dont do path resolution for special characters    
    if(tempArr[k][0]=='|' || tempArr[k][0]=='<' || tempArr[k][0]=='>' || tempArr[k][0]=='&')
      {}
    else
      {
        int len=strlen(tempArr[k]);
        tempArr[k][len]='\0';
        for(i=0; i < strlen(tempArr[k]);i++){
          if(tempArr[k][i]=='\n')
            tempArr[k][i]='\0';
        }

   
  //check if it's a built in
  if(strcmp(tempArr[k],"exit")==0 || strcmp(tempArr[k],"io")==0 || strcmp(tempArr[k],"echo")==0 || strcmp(tempArr[k],"etime")==0){
   
    //do not expand the command, do not expand arguments - do nothing
    
  }


  //check if it's cd then call convert_path
  else if(strcmp(tempArr[k],"cd")==0){
    k++;
    for(i=0; i < strlen(tempArr[k]);i++){
      if(tempArr[k][i]=='\n')
        tempArr[k][i]='\0';
    }
    cmd[k]=convert_path(tempArr[k]);
   
    
  }
  
  //check if it's external - get full pathname and save in cmd[0]
  else{
    
    int m;
    int exist;
   
    struct stat buf;
    //search paths, if in one of them, return 0
    for(i=0; i<20; i++){
      temp= calloc(strlen(split_path[i])+strlen(tempArr[k]), 1);
      strcpy(temp,split_path[i]);
      strcat(temp, "/");
      strcat(temp,tempArr[k]); 
     
    
      
      //some reason \n is getting stored in some commands - so get rid of it
      for(m=0; m < strlen(temp); m++){
        if(temp[m]=='\n'){
          temp[m]='\0';
        }
      }
     
      
     
      exist=stat(temp, &buf);

      
     
      if(exist==0){
        cmd[k] = (char*) calloc(strlen(temp),1);
        //      cmd[k]=NULL;
        strcpy(cmd[k],temp);
        temp=NULL;
        
        
        break;
      }
      

    }
  

  }
      
  
      }
  }
   
  // for(i=0; i< count; i++)
  // printf("cmd[i] is now %s\n", cmd[i]);

  
  

  
  return cmd;
}
//resolve_paths calls this
char *convert_path(char *cmd){
  
  int i,j, k, l, t;
  char *temp;
  //cwd for future use
  char *cwd = getenv("PWD");

  //remove the first /
  if(cwd[0]=='/'){
    for(i=0; i < strlen(cwd); i++)
      cwd[i]=cwd[i+1];
  }
  //getting home variable
  char * home=getenv("HOME");
  if(home[0]=='/'){
    for(i=0; i < strlen(home); i++)
      home[i]=home[i+1];
  }

  char *pch;
  char *parent;
  //for getting the parent of cwd
  pch=strrchr(cwd, '/');                                                    
  parent= (char*) malloc(sizeof(cwd));                                        
  int p = pch-cwd+1;                                                        
                                                      
  for(j=0; j < pch-cwd; j++){                                               
    parent[j]=cwd[j];                                                         
  }                                  
  parent[j+1]='\0';                                       
                                   
   

    //add null delimiter to all strings - should integrate this into parsing
    int len = strlen(cmd);
    cmd[len]='\0';
     
  
    char buffer[250];
    l=0;
    int count=0; //increment to 1 if match is found
    //check if it's just '.'
    if(strcmp(".", cmd)==0){                                            
      strcpy(buffer, cwd);
      l=strlen(cwd);
      count=1;                                                      
    }   
    //check if it's just '..'
    else if(strcmp("..", cmd)==0){
      strcpy(buffer, parent);
      l=strlen(parent);
      count=1;
      
    }
   
    else if(strcmp("~", cmd)==0){
      //for home dir - if no path                                             
      strcpy(buffer,home);                                        
      l=strlen(home);
      count=1;
  
    }

    //checking if ~ starts string
    if(cmd[0]=='~' && cmd[1]=='/'){
      count=1;
      //copies home path
      for(j=0; j < strlen(home); j++){
        buffer[l]=home[j];
        l++;
      }
      //copies rest of path
      for(k=1; k < strlen(cmd);k++){
        buffer[l]=cmd[k];
        l++;
      }
    }   
    if(count==0){
      for(j=0; j < strlen(cmd); j++){
      
        //if . is found, replace with cwd
        if(cmd[j]=='.' && (cmd[j-1]=='/' || cmd[j+1]=='/') 
           && cmd[j-1]!='.' && cmd[j-1]!='.' ){
       
          count=1;
          for(k=0; k < strlen(cwd); k++){
            buffer[l]=cwd[k];
            l++;
                
          }
        }
        //if . not found, copy cmd[i] to buffer
        else{
          if(cmd[j]!='.'){    
            buffer[l]=cmd[j];
            l++;
          }
        }
      
      }
    }  
    //if no match yet 
    if(count==0){   
      l=0;  
      for(j=0; j < strlen(cmd); j++){
        //for ..
        if(cmd[j]=='.' && cmd[j+1]=='.' 
           && (cmd[j-1]=='/' || cmd[j+2]=='/')){
            

          count=1;
          for(k=0; k < strlen(parent); k++){
            buffer[l]=parent[k];
            l++;
          }
          j++;
        }
        //if .. not found, copy cmd[i] to buffer                              
        else{
           
          buffer[l]=cmd[j];
          l++;
            
            
        }
      }
        
      
                
    }
    //got through all of them and still no match - must be file or dir
    if(count==0){
      //validate directories/files                              
      int validity= chdir(cmd);                                        
      if(validity==-1)                                                     
        printf("\n%s is not a valid file/directory\n", cmd);                   
 

    }
 
        
    buffer[l+1]='\0';                                               
          
    cmd = (char*) malloc(strlen(buffer));
    strcpy(cmd,buffer);
    cmd[l+1]='\0';

    
  return cmd;




}

char **expand_variables(char **args)
{
	/*
	$PATH
	$HOME
	$USER
	$SHELL
	$PWD
	*/
	int i;
	for (i = 0; i < arg_size; ++i) 
        printf("%s\n", args[i]);

	return NULL;
}

char *my_read () 
{
/**************************************
Function to et line of data from stdin
***************************************/ 

	// char *toParseStr = (char*)malloc(10);
	// printf("Enter string here: \n");
	// scanf("%s",toParseStr);
	// printf("%s\n",toParseStr);
	// free(toParseStr);
	// char *line = NULL;
 	// ssize_t bufsize = 0; // have getline allocate a buffer for us
 	// getline(&line, &bufsize, stdin);
	// char *line[MAXCHAR];
 	// fgets(line, MAXCHAR, stdin);
 	// printf("%s\n",line);
  	// return line;
 	
 	char *command;
 	command=(char*) malloc(MAXCHAR);
  	//int i;
 	fgets(command, MAXCHAR,stdin);
 	// printf("%s\n",command);
	return command;
}
//builtin handling
void builtins(char **cmd){

  //check exit
  if(strcmp(cmd[0], "exit")==0){
    printf("\nExiting Shell...\n");
    exit(0);
  }

  //check cd
  else if(strcmp(cmd[0],"cd")==0){
    char *path;

    /*need to fix error checking
      if(cmd[2]!=" " && cmd[2]!="\0" && cmd[2]!= "" && cmd[2]!="\n")
        printf("\nError: More than one argument is present\n");
    */

    /* If no arguments supplied, it behaves as if $HOME is the argument*/
    //not working yet
    if(cmd[1]=="\0" || cmd[1]=="" || cmd[1]==" " || cmd[1]=="\n"){
      path=getenv("HOME");
      printf("IN HOME");
    }
    else{
      path=cmd[1];
    }

    int ret=chdir(cmd[1]);
    if(ret==-1)
      printf("Error: Invalid directory\n");
    else{
      setenv("PWD", cmd[1], 1);
      //printf("PWD is now %s\n", getenv("PWD"));
    }
  }
  //check echo - need to fix the loop number
  else if(strcmp(cmd[0],"echo")==0){
    int i;
    for(i=1; i < 5; i++){
      // Warning:
      //printf(cmd[i]);
      // Changed to:
      printf("%s",cmd[i]);
      printf(" ");
    }
    printf("\n");
  }

  //check etime
  //doesn't work yet
  else if(strcmp(cmd[0], "etime")==0){
    /*    struct timeval{
      time_t tv_sec;
      suseconds_t tv_usec;
    }

    //stores current time into tv and current timezone in tz
    gettimeofday(timeval *tv, strucut timezone *tz);

    //output time2-time1
    int time_sec = time2.tv_sec - time1.tv_sec;
    int time_usec=time2.tv_usec -time1.tv_usec;

    printf("Elapsed Time: %i.%i\n", time_sec, time_usec);*/
  }
  //checks io
  else if(strcmp(cmd[0], "io")==0){
    //execute supplied commands
    //record /proc/<pid>/io while it executes
    //when finished, output each recorded values
  }

}
int my_execute (char ** cmd) 
{
  int status;
  pid_t pid = fork();
  if(pid <0)
  {
    // faillure child process not creatd
    return -1;
  }
  else if(pid == 0)
  {
    // Child process
    execv(cmd[0], cmd);
    return 0;
  }
  else
  {
    // We are in parent process and parent needs to wait. Honestly I am not sure why but it works
    waitpid(pid, &status,0); 
    return pid;
  }

  //return 0;
}

void my_clean () {}
