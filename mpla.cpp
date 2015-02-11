#include <iostream>
#include <unistd.h>     
using namespace std;

int main(void)
{
    for(int i=0 ; i <100 ; i++)
    {
	cout<< "sleeping " <<endl;
        sleep(10);
    }
}
