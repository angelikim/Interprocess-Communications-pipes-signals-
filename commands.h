#define SIZE 1000000


class Executor
{

    class List 
    {
    struct Node
    {
	    int pid;
	    std::string job;
        struct Node * next;
    } ;


        int size;
        Node *first,*last;
        
    
 public:
    
    List () ;
    
    ~List();
 	
    int  size_List () ;

    void  push_List ( int , std::string );

    int remove_from_List ( std::string * , int ) ;//diagrafi mono ap to telos
    

    std ::string print_List();
    
    int get_pid( Node *);
    
    std::string  get_job( Node *);
     
    
    Node * return_last();
    
    Node * return_first();
    
    

    };


    unsigned int concurrency;
    List  waiting;
    List  running;


 public:
   
    Executor();
    ~Executor(){};
    
    int get_concurrency();
    void set_concurrency( int );
   
    List * get_waiting();
    List * get_running();
    
    void push_waiting( int ,std:: string  );
    void push_running( int  , std::string );
    
    std::string print_waiting();
    std::string print_running();
    
    int remove_waiting(std:: string *,int data = -1);
    int remove_running(std:: string *,int data = -1);
  
    


};

extern int file_commander,fd_commander;
extern std :: string buff;
extern int fd_executor,file_executor;
extern Executor ex; 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int write_executor( std:: string buffer);
  
std :: string read_executor( );

void signal_read (int );

void signal_open (int);


void close_pipe(int);

void children_handler( int);

void run_waiting();

std :: string read_commander( );

int write_commander( char ** , int  , int );

void create_server(int * );






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////















