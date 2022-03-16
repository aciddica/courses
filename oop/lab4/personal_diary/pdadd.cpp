#include"diary.h"
void InputNewEntity(Diary &NewOne);
std::vector<Diary> my_diary;
void InputDiaries(std::vector<Diary> &Diaries); 
void StoreDiaries(std::vector<Diary> &Diaries);
int main()
{
    
    my_diary.clear(); //initialize    
    Diary NewEntity;
    InputDiaries(my_diary);
    InputNewEntity(NewEntity);
    int exists = 0; //if there is a same date diary
    for(int i = 0;i < my_diary.size(); i++)
    {
        if(NewEntity.date == my_diary[i].date) //replace the same date entity
        {
            exists = 1;
            my_diary[i] = NewEntity;
            break;
        }
        
    }
    if(!exists) my_diary.push_back(NewEntity); //otherwise add new diary
    StoreDiaries(my_diary); //store back the my_diary
    return 0;

}

void InputNewEntity(Diary &NewOne)
{
    ifstream fin2;
    fin2.open("pdadd_test.txt");
    string Cur_Line;
    if(!fin2.eof()) getline(fin2, Cur_Line);
    NewOne.date = Cur_Line;
    if(!fin2.eof()) getline(fin2, Cur_Line);
    while(!fin2.eof())
    {
        NewOne.text.push_back(Cur_Line);
        getline(fin2, Cur_Line);
    }
    NewOne.text.push_back(Cur_Line);
    fin2.close();
    return ;
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
