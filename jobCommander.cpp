#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <fcntl.h>
#include <cerrno>
#include <fstream>
#include <sys/stat.h>
#include <cstring>
#include <signal.h>

#include "commands.h"



using namespace std;

extern int file_commander,fd_commander;





int main ( int argc , char ** argv)
{
    int pid;
    ifstream executor_file("executor.txt");
    bool exists;
    int exec_id;
    if(executor_file) 
    {
       
        exists=true;
        executor_file>>exec_id;
        executor_file.close();
        
    }
    else
    {
        exists=false; 
    }     
	switch (argc) 
	{
		case 1:
		{
			cout<< "Wrong parameters..."<<endl;
			break;
		}
		case 2:
		{
			
			if(!strcmp( argv[1] ,"exit" ) )
			{
				if(!exists)
				    cout << " executor is currently not running " <<endl;
				else if(  write_commander( argv  , argc, exec_id) == 1 )
			            cout <<"Failed while writing in pipe " <<endl;	
			}
			else
			    cout<<"wrong command , try again"<<endl;
			break;
		}
		default:
		{
			if( (!strcmp( argv[1] , "issuejob" ))  )
			{
			    if (!exists)
			    {  
			        create_server(&pid);
			        if( pid < 0 )
			        {
			            cout<<"Failed while exec"<<endl;
			            
			        }
			        
			        exec_id=pid;
			    }
			    
			    if(  write_commander( argv  , argc , exec_id ) == 1 )
			        cout <<"Failed while writing in pipe " <<endl; 
			    
			    cout<<read_commander()<<endl; 
                    
			   
			}	
			else if(!strcmp ( argv[1], "setConcurency") )
		    {   
		        if (!exists)
			    {
			        create_server(&pid);
			        if( pid < 0 )
			            cout<<"Failed while exec"<<endl;      
			        exec_id=pid;
			    }
			    
			    if(  write_commander( argv  , argc, exec_id) == 1 )
			        cout <<"Failed while writing in pipe " <<endl;     
			    
			   
			}
			else if(!strcmp( argv[1], "stop" ))
			{   
		        if (!exists)
			    {
			            cout<<"no processes running .."<<endl;
			            
			                 
			    }
			    
			    if(  write_commander( argv  , argc, exec_id) == 1 )
			        cout <<"Failed while writing in pipe " <<endl;     
			    
			   
			}
	        else if(!strcmp( argv[1], "poll" ))
			{   
		        if (!exists)
			    {
			            cout<<"no processes running .."<<endl;      
			             
			    }
			    else
			    {
			        if(argc !=3)
			            cout<<"wrong command"<<endl;
			        if( (!strcmp(argv[2],"queued")) || (!strcmp(argv[2] , "running")) )
			         {
			            if(  write_commander( argv  , argc, exec_id) == 1 )
			                     cout <<"Failed while writing in pipe " <<endl;     
			        
			            cout<<read_commander()<<endl;
			        }
			        else
			            cout<< "wrong command " <<endl;
			    }   
			}
			else
			    cout << "wrong command" <<endl;
			
			    
        }
	}
    
    
   
	return 0;
}


