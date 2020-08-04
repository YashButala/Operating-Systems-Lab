#include<bits/stdc++.h>
#include<sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<grp.h>
#include<errno.h>
#include<dirent.h>
#include<sys/stat.h>
#include<pwd.h>
#define pathlim 1000
#define UBound 100
#define EXIT_FLAG 0
using namespace std;


// find the index of <
int inputPos(char *enter)
{
	int itr=0;
	while(enter[itr]!='\0')
	{
		if(enter[itr]=='<')
			return itr;
		else 
			itr++;
	}
	return -1;
}

// find the index of >
int outPos(char *enter)
{
	int jtr = 0;
	while(enter[jtr]!='\0')
	{
		if(enter[jtr]=='>')
			return jtr;
		else
			++jtr;
	}
	return -1;
}


// break a string according to a delimiter
int string_delimiter(char **argument,char *str,const char *delim)
{
	int i=0;
	argument[i]=strtok(str,delim);
	for(;argument[i]!=NULL;)
		argument[++i]=strtok(NULL,delim);
	return i;
}


// execute external command 
void execExtCommand(char *enter)
{

	char *words[UBound];
	//If string is exit then exit
	if(!strcmp(enter,"exit"))
	{		
		cout<<"\033[1;31m\nError in execution\033[0m\n"<<endl;
		exit(0);
	}

	string_delimiter(words,enter," \n");

	//execute the  other program
	execvp(words[0],words);
	cout<<"\033[1;31m\nError in executing the file.\033[0m"<<endl;
	kill(getpid(),SIGTERM); //kill the child process

}

void executeIO(char *enter)
{

	char *words[UBound];  // breaks the string 
	char *files[UBound]; // stores argument by removing unwanted spaces
	int in=inputPos(enter), out=outPos(enter);
	// break enter according &, <, > and \n and get size of words in i
	int i=string_delimiter(words,enter,"&<>\n");
	int infiledes,outfiledes;
	pid_t p=0;
	
	if(p==0)
	{
		if(in==-1 && out==-1)
		{
			if(i<=1)
			{
				execExtCommand(words[0]);
			}
			else
			{ 
				cout<<"\033[1;31m\nError!\n Unexpected arguments\033[0m\n"<<endl;
			}
		}
		else if(in>0 && out==-1)
		{
				int f=string_delimiter(files,words[1]," \n");
				//cout<<words[1]<<endl;
				if(f!=1)
				{
					cout<<"\033[1;31m\nInput file error\033[0m\n"<<endl;
					return;
				}
				//cout<<files[0]<<endl;
				infiledes = open(files[0], O_RDONLY);
				if(infiledes<0)
				{
					cout<<"\033[1;31m\nError when opening enter file\033[0m\n"<<endl;
					return;
				}

				//redirect STDIN to the mentioned enter file
				close(0);
				dup(infiledes);
				close(infiledes);
			 
		}
		else if(in>0 && out == -1)
		{
			cout<<"\033[1;31m\nError!\033[0m\n"<<endl;
		}
		else if(in==-1 && out>0 && i==2)
		{
			int f=string_delimiter(files,words[1]," \n");
			if(f!=1)
			{
				cout<<"\033[1;31m\nOutput file error\033[0m\n"<<endl;
				return;
			}
			outfiledes = open(files[0], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(outfiledes<0)
			{
				cout<<"\033[1;31m\nError in opening output file\033[0m\n"<<endl;
				return;
			}

			//redirect STDOUT to the mentioned output file
			close(1);
			dup(outfiledes);
			close(outfiledes);
			
		}
		else if(in == -1 && out>0 && i!=2)
		{
			cout<<"\033[1;31m\nError!\033[0m\n"<<endl;
		}
		else if(in>0 && out>0 && i==3)
		{
			int f;
			if(in>out) //output file entered first
			{
				f=string_delimiter(files,words[1]," \n");
				if(f!=1)
				{
					cout<<"\033[1;31m\nError in output file\033[0m\n"<<endl;
					return;
				}
				outfiledes = open(files[0], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				f=string_delimiter(files,words[2]," \n");
				if(f!=1)
				{
					cout<<"\033[1;31m\nError in output file\033[0m\n"<<endl;
					return;
				}
				infiledes = open(files[0], O_RDONLY);
			}
			else if(in<out) //enter file entered first
			{
				f=string_delimiter(files,words[2]," \n");
				if(f!=1)
				{
					cout<<"\033[1;31m\nError in output file\033[0m\n"<<endl;
					return;
				}
				outfiledes = open(files[0], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

				f=string_delimiter(files,words[1]," \n");
				if(f!=1)
				{
					cout<<"\033[1;31m\nError in output file\033[0m\n"<<endl;
					return;
				}
				infiledes = open(files[0], O_RDONLY);				
			}

			//redirect STDIN and STDOUT to mentioned files

			close(0);
			dup(infiledes);
			close(1);
			dup(outfiledes);
			close(infiledes);
			close(outfiledes);
		}
		else 
			return;
		execExtCommand(words[0]);
	}
	return;
//	exit(0);
}

void execPipe(char** words, int N)
{

	//define N-1 pipes
	int i=0,p[N-1][2];

	while(i<N-1)
	{
		//create pipe
		if(pipe(p[i])<0)
		{
			cout<<"\033[1;31m\nPipe creation failed\033[0m\n"<<endl;
			return;
		}	
		i++;
	}

	//create N child processes
	pid_t pd;
	for(int i=0;i<N;i++) 
	{ 
        if((pd=fork()) == 0) 
        { 
        	//this is the child process
        	//read from the previous pipe if it exists
        	if(i!=0)
        		dup2(p[i-1][0], 0);		//redirect STDIN to read end of the previous pipe	
			if(i+1!=N)
        		dup2(p[i][1], 1);
        	int j =0;
			while(j<N-1)
			{	
				close(p[j][0]);
				close(p[j][1]);
				j+=1;
			}
        	executeIO(words[i]);
        	exit(0); 
        }
		else 
		{
			usleep(10000);
			int j = 0;
			while(j<i)
			{
				close(p[j][0]);
				close(p[j][1]);
				j+=1;
			}
		}
    }
	while(wait(NULL)>0);
	exit(0);
}
void terminate()
{
	
	return;
}		
int main()
{

	//print welcome string in green colour
	cout<<"\033[1;32mWelcome to os labs :\033[0m\n";

	while(99)
	{

		//strings for I/O and for holding other data
		char path[pathlim]="",enter[pathlim]="", *ptr;

		//Get current directory for printing in the bash
		ptr = getcwd(path, sizeof(path));		

		//error in getcwd
		if(ptr==NULL) 
		{
			perror("Error in getting the current directory");
			continue;
		}
		else
		//print present working directory in blue colour
			cout<<"\033[1;34m"<<path<<"\033[0m"<<"$ ";

		//get command from user
		fgets(enter, 1000, stdin);	

		//check for exit condition
		if(strlen(enter)>=3 && enter[0]=='e' && enter[1]=='x'  && enter[2]=='i' && enter[3]=='t')
		{
			cout<<"\033[1;31m\nExiting from the shell\033[0m\n"<<endl;	
			exit(0);
		}
		
		
		//flag to check for & in the command
		int p=0,backrun = 0,j,i=0;

		//checking & character followed by any number of spaces

		while(enter[p]!='\0')
		{
			if(enter[p]=='&')
			{
				j = p;
				j++;
				while(enter[j]==' ')
					j++;
				
				if(enter[j]=='\n')
				{
					enter[p++] = '\n';
					backrun = 1;
					enter[p] = '\0';
				}
			}
			p++;
		}

		//for the pipe commands' arguments
		char *words[UBound];

		//checking the existence of pipes in the entered command

		//separating individual commands by |
		words[i]=strtok(enter,"|");
	
		while(words[i]!=NULL)
			words[++i]=strtok(NULL,"|");
		

		if(fork()==0)
			execPipe(words, i);
		else 
		{
			//waiting for child process to exit if backrun is 0	
			if(!backrun)
			{
				while(waitpid(-1,NULL,0)>0);
			}
		}
	}
	return 0;
}
