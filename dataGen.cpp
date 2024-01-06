#include <bits/stdc++.h>
using namespace std;

int main()
{
    ofstream output("../data/big1.csv");

    output<<"Sid,Age,Sex,Height,Weight\n";
    srand(time(NULL));
    for(int i=0;i<2560;i++)
    {
        output<<i+1<<","<<rand()%100<<","<<rand()%2<<","<<rand()%200<<","<<rand()%100<<endl;
    }
    output.close();
    return 0;
}