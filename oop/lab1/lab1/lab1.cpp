#include<iostream>
#include<iomanip>//control output format
#include<string>
#include<fstream>
using namespace std;
int main()
{
	ifstream fin; 
    fin.open("in.txt");
    char *name[20];
    int score1[20],score2[20],score3[20];
    double avg1=0,avg2=0,avg3=0;//the variables we use later
    double min1=99999,max1=-1,min2=99999,max2=-1,min3=99999,max3=-1;
    
    for(int i=1;i<=10;i++)
    {
        name[i]=new char[10];//allocate the memory for name  
        fin>>name[i]>>score1[i]>>score2[i]>>score3[i];
        avg1+=score1[i];//calculate the sum
        avg2+=score2[i];
        avg3+=score3[i];
        if(score1[i]>max1) max1=score1[i];//update the maximal and minimal value
        if(score2[i]>max2) max2=score2[i];
        if(score3[i]>max3) max3=score3[i];
        if(score1[i]<min1) min1=score1[i];
        if(score2[i]<min2) min2=score2[i];
        if(score3[i]<min3) min3=score3[i];
        
    }
    cout<<"no      name    score1  score2  score3  average"<<endl;//output the table title
    for(int i=1;i<=10;i++) cout<<std::left<<setw(8)<<i<<setw(8)<<name[i]<<setw(8)<<score1[i]<<setw(8)<<score2[i]<<setw(8)<<score3[i]<<setw(8)<<(score1[i]+score2[i]+score3[i])/3.0<<endl;//output the student information and his/her average score
    cout<<std::left<<"        average "<<setw(8)<<avg1/10<<setw(8)<<avg2/10<<setw(8)<<avg3/10<<endl;//output the average,minimal and maximal score in the right format
    cout<<std::left<<"        min     "<<setw(8)<<min1<<setw(8)<<min2<<setw(8)<<min3<<endl;
    cout<<std::left<<"        max     "<<setw(8)<<max1<<setw(8)<<max2<<setw(8)<<max3<<endl;
    fin.close();
    return 0;
}
