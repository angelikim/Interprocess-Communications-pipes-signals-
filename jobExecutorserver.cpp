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


extern string buff;
extern int fd_executor,file_executor;
extern Executor ex; 


     
      

int main (int argc , char ** argv)
{

    
    int count=1 , i=0, waiting_id=0;
    char ** part;
    char fifo_write[]="fifo_write";
    char fifo_read[]="fifo_read";
    string str;

    
    static struct sigaction act , act1 ,act2 , act3;
    
    act.sa_handler=signal_read;
    sigfillset(&(act.sa_mask));
    act.sa_flags=SA_RESTART;
    sigaction(SIGRTMIN , &act , NULL);
    
    act1.sa_handler=signal_open;
    sigfillset(&(act1.sa_mask));
    act1.sa_flags=SA_RESTART;
    sigaction(SIGRTMIN +1 , &act1 , NULL);
    
    act2.sa_handler=close_pipe;
    sigfillset(&(act2.sa_mask));
    act2.sa_flags=SA_RESTART;
    sigaction(SIGRTMIN +2 , &act2 , NULL);

    act3.sa_handler=children_handler;
    sigfillset(&(act3.sa_mask));
    act3.sa_flags=SA_RESTART;
    sigaction(SIGCHLD , &act3 , NULL);

    ofstream outfile ("executor.txt");
    outfile<<getpid();
    outfile.close();
  
   while(1)
    {    
         
        if( (ex.get_waiting()->size_List() == 0) && (buff.empty()) ) 
           pause();
           
        if(buff.empty())
        {  
            run_waiting();
            continue;
        }
            
        count=1 ;
        i=0;
        istringstream str_stream(buff);
             
        while(buff[i] != '\0'  )
        {
            if(buff[i] == '#')
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
        if(!strcmp(part[0],"issuejob"))
        {    
            char  temp[SIZE];     
            if( ex.get_running()->size_List() < ex.get_concurrency() &&  (ex.get_waiting()->size_List() == 0) )
            {  
                int pid=fork();
                int status =0 ;
                
                
			    if(pid < 0 ) 
			    {
			        cout<<"unable to fork "<<endl;
			    }	
			    else if (pid == 0 )
			    {   
                    char ** part2;
                    part2 = new char * [count-1 ];
                    
                    for(i= 0 ; i  < count ; i ++ )
                        part2[i]= new char[15];
                    
                    for( i=0 ; i< count -2 ; i ++)
                        part2[i]=part[i+1];
                    part2[count -2] =NULL;
                    
			        execvp(part[1] ,part2);
			        perror("exec");
                    _exit(status);
			     }
			     else
			     {   
			        sprintf (temp, "%d",pid);
			        write_executor("<"+ buff+ (string) temp   +"#"+"RUNNING>");
			        ex.push_running( pid , buff );
			     }  
                        
                    
            }
            else 
            {
                 waiting_id++;
                 sprintf (temp, "%d",waiting_id);
                 write_executor("<"+ buff +"QUEUED >   (not actual pid)");
                 ex.push_waiting( waiting_id, buff);
            } 

            buff.clear();
       }
       else if(!strcmp(part[0],"setConcurency"))
       {
            if(count!=3)
                cout<< " wrong arguments"<<endl;
            else
            {  
                if( atoi( part[1])  >0)
                {
                    ex.set_concurrency ( atoi(part[1] ) );
                    cout<<" prin tin run waiting " <<endl;
                    run_waiting();
                    cout<< " meta tin run waiting " <<endl;
                }   
                else
                    cout<< "concurency must be greater than zero "<<endl;
            }
            buff.clear();
       }
       else if(!strcmp(part[0],"stop"))
       {
            int rtn;
            string  job_id ;
            buff.clear();
            rtn = ex.remove_waiting(&job_id ,atoi(part[1]));
                
            if(rtn == -1)
                rtn=ex.remove_running(&job_id ,atoi(part[1]) ); //isos edo prepei na steilo sima stin diergasia kill
            
       }
       else if(!strcmp(part[0],"poll"))
       {  
        buff.clear();
        if( !strcmp (part[1] , "running") )
            write_executor( ex.print_running()) ;
        if( !strcmp (part[1] , "queued" ) )
            write_executor(ex.print_waiting());
        }
        else if(!strcmp(part[0],"exit"))
        {
            while (true)
            {   cout<< " killing" <<endl;
                int status;
                pid_t done = wait(&status);
                if (done == -1) {
                    if (errno == ECHILD) break; // no more child processes
                }
                    else
                    {
                        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
                        {
                            cerr << "pid " << done << " failed" << endl;
                            
                        }
                    }
                }      
               break;
        }
        
        if(part !=NULL)
        {   cout<<"edo "<<endl;
            for( i = 0; i < count;i++) 
                delete[] part[i];
            delete [] part; 
            part=NULL;    
           
        }
        
           
     }
     
     if( remove("executor.txt") )
        perror("error removing file");  
     unlink(fifo_write);
     unlink(fifo_read);
     flush(cout);
    exit (1);
}
              

