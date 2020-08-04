#include <bits/stdc++.h>
	#include <iostream>
	#include <cstdio>
	#include <pthread.h>
	#include <cstdlib>
	#include <ctime>
	#include <signal.h>
	#include <unistd.h>
	#include <map>
	using namespace std;

	#define JOBCOUNT 1000
	#define MAX  5000

	void* doJobs(void*);
	void toSleepOrNot(int);

	pthread_mutex_t buf_lock;

	int Pcount=0,Ccount=0;
	int SLP = 0, EXEC = 1,ENDS = 2;
	int TN, prodFlag = 0,completed =0;
	bool Noconsumer=false;


	int compconsumer=0;


	struct cSWITCH
	{
		int dest,source;
		bool flag;
	};


	struct STATUS
	{
		int* 			status;
		cSWITCH 	sswitch;
		int 			threadFlag;
	};
	queue<int> buffer;


	STATUS statobj;
	struct myThreads
	{
		pthread_t 	tid;				
		char 		type;					
		int 		threadNum;			
		int 		state;				
	};
	void initialize(myThreads* mth,int i)
	{
		
		if((rand() % 2)==0)
		{
			mth->type = 'P';
			Pcount++;
		}
		else
		{
			mth->type = 'C';
			Ccount++;
		}

		if(Pcount==TN)
		{
			Noconsumer=true;
		}
		
		mth->threadNum = i;
		mth->state = 0;
		pthread_create(&mth->tid, NULL, doJobs, (void *) mth);
	}

	void join(myThreads* mth)
	{
		pthread_join(mth->tid, NULL);
	}

	void resume(myThreads* mth)
	{
		pthread_kill(mth->tid, SIGUSR2);
	}



	myThreads *Workers;				
	map <pthread_t, int> m;
	void SignalHandler()
	{
		
		signal(SIGUSR1, toSleepOrNot);
		signal(SIGUSR2,	toSleepOrNot);
	}
	void toSleepOrNot(int signo)
	{

		SignalHandler();
		if(signo==SIGUSR1)
		{
			Workers[m[pthread_self()]].state = SLP;
			statobj.status[m[pthread_self()]] = SLP;
			pause();
		
		}
		else if(signo==SIGUSR2)
		{
			statobj.status[m[pthread_self()]] = EXEC;
			Workers[m[pthread_self()]].state = EXEC;
		}
	}

	void* doJobs(void *arguments)
	{
		myThreads *t;
		int num;
		t = (myThreads *) arguments;
		SignalHandler();
		pause();
		num= t->threadNum;

		if(Workers[num].type == 'C')
		{
			while(!completed)
			{

				if(buffer.size())
				{
					
					pthread_mutex_lock(&buf_lock);
					if(buffer.size()>0)
					{
						buffer.pop();
					}
					pthread_mutex_unlock(&buf_lock);	
					usleep(1000);
				}
				else
				{
					statobj.status[num] = Workers[num].state = SLP;
				}
				
			}
		
		}
		else if(Workers[num].type == 'P')
		{
			int jobsGenerate = JOBCOUNT;
			while(jobsGenerate)
			{
				if(buffer.size()>=MAX && Noconsumer)
				{
					break;
				}

				if(buffer.size()<MAX)
				{

					pthread_mutex_lock(&buf_lock);
					if(buffer.size()<MAX)
					{	
						buffer.push(rand()%100);
						jobsGenerate--;
					}	
					pthread_mutex_unlock(&buf_lock);
					usleep(1000);
				}
				else
				{
					statobj.status[num] = Workers[num].state = SLP;
				}

			}
		}
		if(Noconsumer && buffer.size()>=MAX)
		{
			pthread_kill(Workers[num].tid,0);
		}

		statobj.status[num] = Workers[num].state = ENDS;
		statobj.threadFlag = num;
		return 0;
	}

	bool Done_check()
	{
		if(buffer.size()==MAX && Noconsumer)
		{
			prodFlag=1;
			return true;

		}
		if(prodFlag == 1)
		{
			return true;
		}
		
		for(int i=0; i<TN; i++)
		{
			if(Workers[i].type=='P') 
				if(Workers[i].state!=ENDS)
					return false;
		}
		

	 	if(buffer.size())
	 		return false;
	 	else
	 		return true;
	}

	void* scheduler(void *arguments)
	{
		while(true) 
		{
			int cnt = 0;
			usleep(1000);

			if(Workers[statobj.sswitch.dest].state!=ENDS)
				pthread_kill(Workers[statobj.sswitch.dest].tid, SIGUSR2);

			usleep(1000000);
			statobj.sswitch.source = statobj.sswitch.dest;
			statobj.sswitch.dest += 1;
			statobj.sswitch.dest %= TN;
			while(statobj.status[statobj.sswitch.dest]==ENDS)
			{
				statobj.sswitch.dest +=1;
				statobj.sswitch.dest=statobj.sswitch.dest%TN;
				cnt+=1;
				if(cnt==TN)
				{
					break;
				}
			}
			if(cnt==TN)
			{
				prodFlag = 1;
			}

			if(Workers[statobj.sswitch.source].state!=ENDS)
			{
				pthread_kill(Workers[statobj.sswitch.source].tid, SIGUSR1);
			}
			statobj.sswitch.flag = true;
			if(Done_check())
				break;
		}
		return 0;
	}
	void* reporter(void *arguments)
	{

		while(true)
		{
			while(!(statobj.sswitch.flag) && (statobj.threadFlag < 0));
			
			if(statobj.sswitch.flag)
			{
				statobj.sswitch.flag = false;
				cout<<"Number of jobs occupying Buffer : "<<buffer.size()<<endl;
				cout<<"Context Switching from ";
				if(Workers[statobj.sswitch.source].type == 'C' && Workers[statobj.sswitch.dest].type == 'C')
				{
					cout<<" consumer thread with index ";
					cout<<statobj.sswitch.source<<" to ";
					cout<<" consumer thread with index ";
					cout<<statobj.sswitch.dest<<" happened "<<endl;
				}

				if(Workers[statobj.sswitch.source].type == 'P' && Workers[statobj.sswitch.dest].type == 'C')
				{
					cout<<" producer thread with index ";
					cout<<statobj.sswitch.source<<" to ";
					cout<<" consumer thread with index ";
					cout<<statobj.sswitch.dest<<" happened "<<endl;
				}
				if(Workers[statobj.sswitch.source].type == 'C' && Workers[statobj.sswitch.dest].type == 'P')
				{
					cout<<" consumer thread with index ";
					cout<<statobj.sswitch.source<<" to ";
					cout<<" producer thread with index ";
					cout<<statobj.sswitch.dest<<" happened "<<endl;
				}
				if(Workers[statobj.sswitch.source].type == 'P' && Workers[statobj.sswitch.dest].type == 'P')
				{
					cout<<" producer thread with index ";
					cout<<statobj.sswitch.source<<" to ";
					cout<<" producer thread with index ";
					cout<<statobj.sswitch.dest<<" happened "<<endl;
				}
				if(Done_check())
				{
					completed = 1;
					break;
				}
			}
			else{
				if(Workers[statobj.threadFlag].type == 'P')
				{
					cout<<"\033[1;36mProducer worker thread that terminated : "<<statobj.threadFlag;
					cout<<"\033[0m"<<endl;
				}
				statobj.threadFlag = -1;
			}
		}
		return 0;
	}
	int main() 
	{
		int i;	
		srand((unsigned)time(NULL));
		/*if(pthread_mutex_init(&stat_lock, NULL))
		{
			cout<<"Error in initialization of the status lock"<<endl;
			return 0;
		}*/
		if(pthread_mutex_init(&buf_lock, NULL))
		{
			cout<<"Error in initialization of the buffer lock"<<endl;
			return 0;
		}

		cout<<"Enter number of threads you want : ";

		//Taking input the value of N from the user
		cin>>TN;
		statobj.sswitch.source = 0;					
		statobj.threadFlag = -1;
		statobj.sswitch.flag = false;
		statobj.sswitch.dest= 0;			

		Workers = new myThreads[TN];
		i=0;
		while(i<TN)
		{
			initialize(Workers+i,i);
			i++;
		}
		i=0;
		while(i<TN)
		{
			m[Workers[i].tid]=Workers[i].threadNum;
			i++;
		}
		statobj.status = new int[TN];
		i=0;
		while(i<TN)statobj.status[i++] = 0;
		pthread_t scheduler_no,reporter_no;
		pthread_create(&scheduler_no, NULL, scheduler, NULL);
		pthread_create(&reporter_no, NULL, reporter, NULL);
		pthread_join(reporter_no, NULL);
		pthread_join(scheduler_no, NULL);
		if(!prodFlag)
		{
			cout<<"\033[1;36mOnly consumer threads are left!\033[0m"<<endl;
		}
		else
		{
			cout<<"\033[1;36mNo consumer thread was found! Jobs won't be consumed!\033[0m"<<endl<<"Jobs could not be consumed. Done!\n";
			return 0;
		}
		i = 0;
		while(i<TN)
		{
			if(Workers[i].type=='C')
				resume(Workers+i);
			i++;
		}
		i = 0;
		while(i<TN)
		{
			join(Workers+i);
			i++;
		}
		cout<<"Successfully completed the jobs"<<endl;
		delete Workers;
		delete statobj.status;

		return 0;
	}
