#include<iostream>
#include<iomanip>//control output format
#include<string.h>
#include<fstream>
using namespace std;
class course//the course class
{
    public:
    char CourseName[20];
    double score;
    course *next;
};
class student//the student class
{
    public:
    char StuName[20];
    int CourseNum;
    course *head=NULL;
    course *tail=NULL;
    student()//initialize the student class
    {
        CourseNum=0;
    }
    student(int num)//initialize the student class with a certain number
    {
        CourseNum=num;
    }
    void add(course node)//add a course information to the class,using linked list
    {
        if(head==NULL)//empty list
        {
            head=new course;
            head->score=node.score;
            strcpy(head->CourseName,node.CourseName);
            tail=head;
            tail->next=NULL;
            head->next=NULL;
        }
        else//add the new node to the list tail
        {
            course *newnode=new course;
            newnode->score=node.score;
            strcpy(newnode->CourseName,node.CourseName);
            newnode->next=NULL;
            tail->next=newnode;
            tail=newnode;
        }
    }
    double average()//calculate the average score
    {
        double sum=0;
        int i=0;
        course *p=head;
        while(p!=NULL)
        {
            sum+=p->score;
            i++;
            p=p->next;
        }
        if(i==0) return 0;//the student has no course
        else return sum/i;
    }

};
student S[1000000];//the array to store students
void Print(int n);//output the student list in the format
int main()
{
    ifstream fin; 
    int stunum;//the number of students
    fin.open("in.txt");
    fin>>stunum;
    for(int i=1;i<=stunum;i++)
    {
        fin>>S[i].StuName>>S[i].CourseNum;
        for(int j=1;j<=S[i].CourseNum;j++)//input all the course information of the student
        {
            course tmp;
            fin>>tmp.CourseName>>tmp.score;//input the course information
            S[i].add(tmp);//add it to the student information
        }
    }
    Print(stunum);
    return 0;
}
void Print(int n)
{
    char * NameList[100000];//record the name information of all students
    int totnum=0;
    for(int i=1;i<=n;i++)//coynt all the distinct courses of all students
    {
        course* p=S[i].head;
        while (p!=NULL)//count all the distinct courses of a student
        {
            int exist=0;
            for(int j=1;j<=totnum;j++)
            {
                if(strcmp(p->CourseName,NameList[j])==0)
                {
                    exist=1;
                    break;
                }
            }
            if(!exist) 
            {
                totnum++;
                NameList[totnum]=p->CourseName;
            }
            p=p->next; 
        }
        
    }
    //output the table title
    cout<<"no      name    ";
    for(int i=1;i<=totnum;i++)
    cout<<std::left<<setw(8)<<NameList[i];
    cout<<"average ";
    cout<<endl;
    
    double CourseInfo[10000][4];//[x][0] store the number of students who take the course,[x][1] stores the sum score,[x][2] stores the max score,[x][3] stores the min score
    for(int i=1;i<=totnum;i++)//initialize
    {
        CourseInfo[i][0]=0;
        CourseInfo[i][1]=0;
        CourseInfo[i][2]=-1;
        CourseInfo[i][3]=99999999;
    }
    for(int i=1;i<=n;i++)//output the student information
    {
        cout<<std::left<<setw(8)<<i<<setw(8)<<S[i].StuName;//output the student name
        course *p=S[i].head;
        double scores[10000];//stores the scores of the student in the certain order
        for(int j=1;j<=totnum;j++) scores[j]=-1.0; //initialize the array
        while(p!=NULL)//output all the course information of the student
        {
            int j;
            for(j=1;j<=totnum;j++)
            {
                if(strcmp(p->CourseName,NameList[j])==0)//if the student have the course
                    {
                        scores[j]=p->score;
                        CourseInfo[j][0]++; //update the course information
                        CourseInfo[j][1]+=p->score;
                        if(p->score>CourseInfo[j][2]) CourseInfo[j][2]=p->score;
                        if(p->score<CourseInfo[j][3]) CourseInfo[j][3]=p->score;
                        break;
                    }
                
            }
            p=p->next;
        }
        for(int j=1;j<=totnum;j++)//output the scores
        {
            if(scores[j]<0) cout<<"        ";	//if don't have the course, output spaces
            else cout<<std::left<<setw(8)<<scores[j];//otherwise output the score
        }
        cout<<std::left<<setw(8)<<S[i].average();
        cout<<endl;
    }
    //output the information of all the students and courses
    cout<<std::left<<setw(8)<<"        average ";
    for(int i=1;i<=totnum;i++)
    cout<<std::left<<setw(8)<<CourseInfo[i][1]/CourseInfo[i][0];
    cout<<endl;
    cout<<std::left<<setw(8)<<"        min     ";
    for(int i=1;i<=totnum;i++)
    cout<<std::left<<setw(8)<<CourseInfo[i][3];
    cout<<endl;
    cout<<std::left<<setw(8)<<"        max     ";
    for(int i=1;i<=totnum;i++)
    cout<<std::left<<setw(8)<<CourseInfo[i][2];
    cout<<endl;
    return ;
    
}
