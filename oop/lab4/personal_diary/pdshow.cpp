#include"diary.h"
std::vector<Diary> my_diary;
void InputDiaries(std::vector<Diary> &Diaries); 
int main()
{
    my_diary.clear(); //initialize    
    InputDiaries(my_diary);
    cout << "please enter the date of the diary you want to find"<<endl; //prompt
    string Specify_date;
    ifstream fin2;
    fin2.open("pdshow_test.txt");
	fin2 >> Specify_date; 
    ofstream fout;
    fout.open("Diaries.txt");
    for(int i = 0;i < my_diary.size();i++) //write back procedure
    {
        if(my_diary[i].date == Specify_date)
        {
            fout << my_diary[i].date << endl;
            for (int j = 0; j < my_diary[i].text.size(); j++)
            {
                fout << my_diary[i].text[j] <<endl;
            }
        }
        
    }
    return 0;
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
