#include "diary.h"
std::vector<Diary> my_diary;
void InputDiaries(std::vector<Diary> &Diaries); 
void StoreDiaries(std::vector<Diary> &Diaries);
int main()
{
    my_diary.clear(); //initialize    
    InputDiaries(my_diary);
    cout << "please enter the date of the diary you want to remove"<<endl; //prompt
    ifstream fin2; 
    fin2.open("pdremove_test.txt");
    string remove_date;
    fin2 >> remove_date;
    int remove_success = 0;
    for(int i = 0; i < my_diary.size(); i++)
    {
        if(my_diary[i].date == remove_date)
        {
            my_diary.erase(my_diary.begin()+i);
            remove_success = 1;
            break;
        }
    }
    StoreDiaries(my_diary); //store back the my_diary
    if(remove_success) return 0;
    else return -1;
}
void InputDiaries(std::vector<Diary> &Diaries)
{
    ifstream fin1;
    fin1.open("Diaries.txt");
    string Cur_Line;
    if(!fin1.eof()) getline(fin1, Cur_Line);
    int NewDiary = 1; //indicate if there is a new diary
    while(!fin1.eof())
    {
        if(NewDiary) //input the next enetity in the present storage
        {
            Diary NextEntity = *new Diary;
            NextEntity.date = Cur_Line;
            NewDiary = 0;
            Diaries.push_back(NextEntity);
        }
        else //add the line to the current diary
        {
            Diaries[Diaries.size()-1].text.push_back(Cur_Line);
        }
        if(Cur_Line == ".") NewDiary = 1;
        else NewDiary = 0;
        getline(fin1, Cur_Line);
    }
    fin1.close();
    return ;
}
void StoreDiaries(std::vector<Diary> &Diaries)
{
    ofstream fout;
    fout.open("Diaries.txt");
    for(int i = 0;i < Diaries.size();i++)
    {
        fout << Diaries[i].date << endl;
        for (int j = 0; j < Diaries[i].text.size(); j++)
        {
            fout << Diaries[i].text[j] <<endl;
        }
        
    }
    fout.close();
    return ;
}
