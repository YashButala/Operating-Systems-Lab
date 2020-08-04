#include<bits/stdc++.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <semaphore.h>
#include <vector>
using namespace std;
using namespace std::chrono;
sem_t m;
struct job
{
	int processid;
	int producerno;
	int priority;
	int jobid;
	int computetime;
};
struct complete{
	struct job alljob[8];
	int buf;
	int jc,jp;
};
int main()
{
	//srand(time(0)); 
	int NP,NC,jobs,shmid;
	key_t key = ftok("/dev/random",'b');
	struct complete* shm;
	//struct shmid_ds *buf;
	int * ptr;
	struct timeval tv;
	vector <int> pids;

	cout<<"Enter number of producers: ";
	cin>>NP;
	cout<<"Enter number of consumers: ";
	cin>>NC;
	cout<<"Enter number of jobs: ";
	cin>>jobs;
	shmid=shmget(key,sizeof(struct complete),IPC_CREAT | 0666);
	shm = (struct complete *)shmat(shmid,NULL,0);
	srand(time(NULL)^getpid()<<16);
	for(int i=0;i<8;i++)
	{
		shm->alljob[i].processid=0;
		shm->alljob[i].producerno=0;
		shm->alljob[i].priority=0;
		shm->alljob[i].jobid=0;
		shm->alljob[i].computetime=0;		
	}
	struct job zero_job;
	zero_job.processid=0;
	zero_job.producerno=0;
	zero_job.priority=0;
	zero_job.jobid=0;
	zero_job.computetime=0;		
	sem_init(&m, 0, 1);
	shm->buf=0;
	shm->jc = 0;
	shm->jp = 0;

	auto start=high_resolution_clock::now();

	for(int i=0;i<NP;i++)
	{
		int x = fork();
		pids.push_back((int)getpid());

	
		srand(time(0) + (int)getpid());
		if(x==0)
		{
		   while(1)
		   {
			
		   		sem_wait(&m);
				sleep((rand()%4));	
				
		   		if(shm->alljob[7].jobid!=0)
					printf("Buffer full ... Waiting for read.\n");
				while(shm->alljob[7].jobid!=0 );
				if(shm->jc>=jobs)
					break;
				if(shm->alljob[7].jobid==0)
				{
					struct job new_job;
					new_job.processid = (int)getpid();
					new_job.producerno = i+1;
					new_job.computetime = rand()%4+1;
					new_job.priority = 1 + rand()%10;
					new_job.jobid = 1 + rand()%100000;
					struct job tmp_job,tmp_job2;
					int flag = 0;
					for (int i1 = 0; i1 < 8; ++i1)
					{
						if(!flag && (new_job.priority)>(shm->alljob[i1].priority))
						{
							tmp_job = shm->alljob[i1];
							shm->alljob[i1]=new_job;
							flag = 1;	
							continue;
						}		
						if(flag)
						{
							tmp_job2 = shm->alljob[i1];
							shm->alljob[i1] = tmp_job;
							tmp_job = tmp_job2;
						}
					}
					shm->buf++;
					shm->jp++;
					
					/*for(int k=0;k<8;k++)
					{
						cout<<shm->alljob[k].jobid<<" "<<shm->alljob[k].priority<<" : ";
					}
					cout<<endl;*/
					cout<<"Producer : Job-ID "<<new_job.jobid<<"\tProducerNo "<<new_job.producerno<<"\tProcessID: "<<new_job.processid<<"\tPriority: "<<new_job.priority<<"\tComputetime: "<<new_job.computetime<<endl;
			  
			    }	
		    	sem_post(&m);
		    }
			exit(0);
		}	
	}
	for(int i=0;i<NC;i++)
	{
		int x = fork();
		pids.push_back((int)getpid());
		srand(time(0) + (int)getpid());
		if(x==0)
		{
			while(1)
			{
				
				sem_wait(&m);
				sleep(rand()%4);
				
				if(shm->jc>=jobs)
				{
					break;
				}

				if(shm->alljob[0].jobid==0)
				{
					printf("Buffer empty ... Waiting for write.\n");
				}
				while(shm->alljob[0].jobid==0 || shm->alljob[0].priority==0 ||shm->alljob[0].processid==0 || shm->alljob[0].producerno==0||shm->alljob[0].computetime==0)
				{
					if(shm->jc>=jobs)
						break;
				}
				if(shm->jc>=jobs)
						break;
				int stime=0;
				if(shm->jp-shm->jc>0)
				{
					shm->jc++;
						
					struct job consumed_job=shm->alljob[0];
					stime = consumed_job.computetime;
				
					for(int j=0;j<7;j++)
					{
						shm->alljob[j]=shm->alljob[j+1];

					}
					shm->alljob[7]=zero_job;
					cout<<"Consumer : "<<"Consumer PID "<<getpid()<<"\tConsumer No "<<i+1<<"\tJob-ID "<<consumed_job.jobid<<"\tProducerNo "<<consumed_job.producerno<<"\tProcessID "<<consumed_job.processid<<
					"\tPriority "<<consumed_job.priority<<"\tComputetime "<<consumed_job.computetime<<endl;
					/*for(int k=0;k<8;k++)
					{
						cout<<shm->alljob[k].jobid<<" "<<shm->alljob[k].priority<<" : ";
					}
					cout<<endl;
					cout<<shm->jp<<" "<<shm->jc<<endl;*/
					shm->buf--;
				}
				sleep(stime);	
				sem_post(&m);
				
		    }		
			exit(0);
		}
	}

	while(shm->jc<jobs);
	auto stop=high_resolution_clock::now();
	auto duration=duration_cast<seconds>(stop-start);
	cout<<"Time Elapsed: "<<duration.count()<<" seconds"<<endl;

	for(int i=0;i<pids.size();i++)
	{
		kill(pids[i],SIGTERM);
	}

		return 0;
}
