#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/stat.h>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <signal.h>

#include "commands.h"

using namespace std;

 string buff;
 int fd_executor,file_executor;
 Executor ex; 
 int file_commander,fd_commander;

int write_executor( string buffer)
{
   	char fifo_read[]="fifo_read";
	
	if(( file_executor = open ( fifo_read , O_WRONLY )) <0 )      
	{
	    perror("error opening fifo_write 1 " );
		return 1;
	}
	
	if (( write ( file_executor , buffer.c_str() , SIZE ) ) == -1)
    {
        perror ( " Error in Writing ") ; 
        return 1 ; 
    }
   
    close(file_executor);
    
    return 0;

} 
 
 
 
string read_executor( )
{   
    
    char  msgbuf[SIZE];
    
    memset(msgbuf,0,SIZE);
  
     if ( read ( fd_executor ,msgbuf ,SIZE )< 0  )
     {
        perror (" problem in reading ") ;
        return "";
     }

     string x(msgbuf);
     
    
     return x;
   
}

void signal_read (int signo )
{
    buff = read_executor();
   

}

void signal_open (int signo )
{
    char fifo_write[]="fifo_write";
        if ( ( fd_executor = open ( fifo_write , O_RDONLY   )) < 0)
     {
        perror (" fifo_write open problem 2 " );
        
     }

}

void close_pipe(int signo)
{
   
    close(fd_executor);
}


void children_handler( int signo)
{
    int status;
    int pid;
    string job_id;
    while ((pid = waitpid((pid_t)(-1), &status, WNOHANG )) > 0)
        ex.remove_running(&job_id,pid);
        
        
    
}



void run_waiting()
{

    int count =1, i=0 , pid,status =0;
    string  part1,str;
    char ** part;
    while(1)
    {   count=1;
        i=0;
        status=0;
        if((ex.get_running()->size_List() < ex.get_concurrency()) && (ex.get_waiting()->size_List() > 0) )
         {
          ex.remove_waiting(&part1,-1);

          if(part1.empty())
            return;
          
          istringstream str_stream(part1);
            while(part1[i] != '\0'  )
            {  
                if(part1[i] == '#')
                    count++;
                i++;
            }
            part = new char * [count ];
            for(i= 0 ; i  < count ; i ++ )
                part[i]= new char[15];
            
            i=0;
            while(getline(str_stream,str,'#'))
            {
                strcpy(part[i],str.c_str());
                i++;
            }
            part[count - 1 ]=NULL;
            for( i =0 ; i < count -1 ; i ++)
                cout <<" thesi " << i << "me timi " << part[i]<<endl;
            pid=fork();
			if(pid < 0 ) 
			{
			    cout<<"unable to fork "<<endl;
			   
			}	
			else if (pid == 0 )
			{   sleep(2);
                char ** part2;
                part2 = new char * [count-1 ];
                for(i= 0 ; i  < count ; i ++ )
                    part2[i]= new char[15];
                for( i=0 ; i< count -2 ; i ++)
                {  
                    part2[i]=part[i+1];
                }
                
                part2[count -2] =NULL;
                sleep(2);
			    execvp(part[1] ,part2);
			    
			    perror("exec");
			     _exit(status);
			             
			}
			else
			{  
			    ex.push_running( pid , part1 );
			    for( i = 0; i < count;i++) 
                    delete[] part[i];
                delete [] part;
                             
			}
             
         }
         else
         {
            break;
         }
     }
     return;

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void create_server(int * pid)
{
    char  fifo_write[] = "fifo_write";
    char fifo_read[]="fifo_read";
    
    *pid=fork();
    if(*pid < 0 ) 
    {
        return;
	}	
    else if (*pid == 0 )
	{ 	           
        char str[] = "./executor";
        char *buff[2];
        buff[1]=NULL;
        buff[0]=str;
		execvp(str ,buff);
		perror("exec");
    }
    else
    {
        if(mkfifo(fifo_write , 0666) == -1)
	    {
	        if(errno != EEXIST)
		    {
		        perror("error while creating fifo_write");
			    return ;
		    }
        }
        if(mkfifo(fifo_read , 0666) == -1)
	    {
	        if(errno != EEXIST)
		    {
		        perror("error while creating fifo_write");
			    return ;
		    }
        }
        sleep(2);
    }    
  
}



string read_commander( )
{   

     char fifo_read[]="fifo_read";
     char  msgbuf[SIZE];
     
     memset(msgbuf,0,SIZE);
     
     if ( ( fd_commander = open ( fifo_read , O_RDONLY   )) < 0)
     {
        perror (" fifo_write open problem 2 " );
        
     }
    
     if ( read ( fd_commander ,msgbuf ,SIZE  )< 0  )
     {
        perror (" problem in reading ") ;
        return "";
     }

     string x(msgbuf);
     close(fd_commander);
    
     return x;
   
}


int write_commander( char ** data , int num , int id)
{
    char  fifo_write[] = "fifo_write";
	int c ;
	string buffer;
	buffer.clear();
	
	for(int i = 1 ; i < num ; i ++ )
	{
	    buffer=buffer+data[i];
	    buffer=buffer+"#";
	}
	
	buffer=buffer+ "\0";

	
    c =kill(id ,SIGRTMIN +1); 
	if(c ==-1)
        cerr<<errno << "(1)" << id <<endl;

	
	if(( file_commander = open ( fifo_write , O_WRONLY )) <0 )
	{
	    perror("error opening fifo_write 1 " );
		return 1;
	}
	
	c =kill(id ,SIGRTMIN); 
    if(c ==-1)
        cerr<<errno<<"(2)" <<endl;
	
	if (( write ( file_commander , buffer.c_str() ,SIZE ) ) == -1)
    {
        perror ( " Error in Writing ") ; 
        return 1 ; 
    }
    
    c =kill(id , SIGRTMIN +2);
    if(c ==-1)
        cerr<<errno<<"(3)" <<endl;
    return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Executor::   Executor()
{
    concurrency=1;
    
}



int Executor::  get_concurrency()
{
    return concurrency;
}



void Executor::   set_concurrency(int data)
{
    concurrency=data;

}



Executor ::  List *       Executor ::  get_waiting()
{
    return &waiting;
}



Executor ::     List *       Executor :: get_running()
{
    return &running;
}




void Executor:: push_waiting(int data , string arg )
{
    waiting.push_List(data , arg );
}




void Executor:: push_running(int data , string arg )
{
 
    running.push_List(data , arg );
}




string Executor :: print_waiting()
{
    return waiting.print_List();
}



string Executor :: print_running()
{
     return running.print_List();
}



int Executor :: remove_waiting(string * job_id,int data)
{
   
   return waiting.remove_from_List( job_id,data );

}



int Executor :: remove_running( string * job_id ,int data )
{
   return running.remove_from_List( job_id,data );
   
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Executor :: List :: List ()    //creating list
{
    
	first=NULL;
	last=NULL;
	size=0;
}



Executor :: List :: ~List()            //deleting list
{
	Node* t ;
		while(size > 0)
		{	
		    t=first;
			first=first->next;
			delete(t);
			size--;
		}
		
}	
		


Executor ::     List    :: Node *   Executor :: List :: return_first () 
{ 
		return first; 
}


Executor ::     List   :: Node * Executor :: List :: return_last () 
{ 
		return last; 
}
	
	
int Executor :: List :: size_List () 
	{ 
		return size; 
	}	



void  Executor :: List :: push_List (int data ,  string arguments )     
	{
		
		if( size == 0)		//  pushing in an empty list ( first element)
		{	
			first = new Node;
			first->next=NULL;
			first->pid=data;
			first->job=arguments;
			last=first;
			size++;	
		}
		else   
		{
			Node *t;
			t=first;      //mpenoun sto first
			first= new Node;    // ara |first|-> |next| -> ...... -> |last| ( to proto pou mpike )
			first->next=t;
			first->pid=data;
			first->job = arguments;
			size++;
		}
     
		return;
	}



// auto xrisimeuei an theleis na bgeis apo tin oura anamonis
int Executor :: List :: remove_from_List (string * job_id,int pid  ) // default timi gia to pid einai -1. Etsi tha to balo na diagrafei ton last 
{                
                                        // ( ton proto pou mpike ) , allios tha  kanei anazitisi tou pid kai tha diagrafei
	int i,megethos;                                              // auto
	Node* t = first , *temp ;
	job_id->clear();
		if((size == 0 ) )
			return -1;			//if list empty or wrong pointer ,error
		if(pid == -1 )
		{   
		    if((size > 1)  )	//if list has more than one nodes
		    {
			    while(1)
			    {
			        if(t->next==last)
			            break;
			        t=t->next;
			    }
                * job_id = last->job;
                
		    	delete  last;
                
			    last=t;
			    
		    }
		   else    // if deleting the only node
		   {	
                * job_id = last->job;
               
			    delete last;
			    first=NULL;
			    last=NULL;
			    	
		   }
		    size--;
		}
		else 
		{//deleting node with specified id
		    bool found = false;
		    int count= 1;
		    while(1)
		    {
		        if(count > size )
		        {
		            found = false;
		            break;
		        }
		        if(t->pid == pid )
		        {
		            found = true;
		            break;
		        }
		        
		        temp=t;
		        t=t->next;
		        count++;
		           
		        
		    }
		    if(!found)
		    {
		        return -1;
		    }
		    else
		    {   
		       
		        if(size =1 )
		        {
		            * job_id = first->job;
                
			        delete first;
			        first=NULL;
			        last=NULL;
			         size --;	
		        
		        } 
		        else
		        {
		            if(t == last)
		            {
		                *job_id=last->job;
                        delete last;
                        last=temp;  
                    }
                    else if(t== first)
                    {
                        temp=first->next;
                        * job_id = first->job;
                        delete first;
                        first=temp;
                    }
                    else
                    {
                        Node *n = t->next;
                         *job_id=t->job;
                        delete t;
                        temp->next=n ;
                    }
                     size --;
                }
		    }
		
		}
	
		return 0;
}


string Executor :: List :: print_List()
{
    Node * t = first;
    string str="";
    char temp[10];
    if(size_List() == 0 )
        str= str + "no jobs available\n"; 
    for(int i =0 ;i < ( size_List()   ) ; i++)
    {   
         sprintf( temp , "%d ",t->pid);
         str = str +t->job + ( (string) temp ) +  "\n";
         t=t->next;
    }
    str=str+"\0";
    return str;
}


int Executor :: List ::  get_pid(Node * node)
{
    return node->pid;
}




string Executor :: List   :: get_job(Node * node)
{
    return node->job;

}










