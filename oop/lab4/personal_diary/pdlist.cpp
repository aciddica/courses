#include"diary.h"
std::vector<Diary> my_diary;
bool cmp(Diary a, Diary b) //used in the sort function, output the diaries in increasing order of date
{
    return a.date < b.date;
}
int main()
{
    cout << "If you want to search diaries in a certain range, please input 1, otherwise input 0"<<endl;
    int choose;
    ifstream fin2; 
    fin2.open("pdlist_test.txt");
    fin2 >> choose;
    my_diary.clear();
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
            my_diary.push_back(NextEntity);
        }
        else //add the line to the current diary
        {
            my_diary[my_diary.size()-1].text.push_back(Cur_Line);
        }
        if(Cur_Line == ".") NewDiary = 1;
        else NewDiary = 0;
        getline(fin1, Cur_Line);
    }
    sort(my_diary.begin(), my_diary.end(),cmp); //sort by the order of date
    string upper, lower;
    if(choose == 1) //if input the boundary
    {
    	cout << "lower bound:"<<endl;
        fin2 >> lower;
        cout << "upper bound:" <<endl;
        fin2 >> upper;
    }
    ofstream fout;
    fout.open("Diaries.txt");
    
    
    for(int i = 0; i < my_diary.size(); i++) //write back procedure
    {
        if(choose == 1) 
        {
        	
            if(my_diary[i].date >= lower && my_diary[i].date <= upper) //only output the entities in the range
            {
                fout << my_diary[i].date << endl;
                for (int j = 0; j < my_diary[i].text.size(); j++)
                {
                    fout << my_diary[i].text[j] <<endl;
                }
            }

        }
        else //output all the entities
        {
            fout << my_diary[i].date << endl;
            for (int j = 0; j < my_diary[i].text.size(); j++)
            {
                fout << my_diary[i].text[j] <<endl;
            }
        }

    }
    my_diary.clear();
    return 0;
}
