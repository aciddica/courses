#include<iostream>
#include<string>
#include<stdlib.h>
#include<time.h>
#include<fstream>
using namespace std;
//the class of room
class Room{ 
    public:
    string name; //the name of the room
    int exits[4]; //the exits of 4 directions, in {up, down, left, right} order
    int exit_num;
    int content; //the content in this room, 0 stands for empty, 1 stands for princess, -1 stands for monster 
    void RoomSet(int up,int down,int left,int right,string Rname)
    {
        exits[0] = up;
        exits[1] = down;
        exits[2] = left;
        exits[3] = right;
        content = 0; //initial to 0
        name = Rname;
        exit_num = up + down + left + right;
    }
    void PrintInfo() //output the prompt and informations
    {   
            cout<<"Welcome to the "<<name<<". There are "<<exit_num<<" exits: ";
            if(exits[3]) cout<<"east ";
            if(exits[2]) cout<<"west ";
            if(exits[0]) cout<<"north ";
            if(exits[1]) cout<<"south ";
            cout<<endl;
            
            cout<<"Enter your command:"<<endl;
        
    }

};
//the class of character
class Char{ 
    public:
    int position; //the room we now in
    int state; //the character state
    Char()
    {
        position = 0;
        state = 0;
    }
};
Room room[9];
Char player;
void Init() //initial the map of the game
{
	srand((unsigned)time(NULL));
    int moster_pos = rand()%8+1;
    int princess_pos = rand()%8+1;
    while(moster_pos == princess_pos) princess_pos = rand()%8+1; //generate the random position of princess and monster and guarentee them not in the same room
    room[0].RoomSet(1, 0, 0, 1, "lobby");
    room[1].RoomSet(0, 0, 1, 1, "room1");
    room[2].RoomSet(0, 0, 1, 0, "room2");
    room[3].RoomSet(0, 1, 0, 1, "room3");
    room[4].RoomSet(1, 0, 1, 1, "room4");
    room[5].RoomSet(1, 0, 1, 0, "room5");
    room[6].RoomSet(0, 0, 0, 1, "room6");
    room[7].RoomSet(0, 1, 1, 0, "room7");
    room[8].RoomSet(0, 1, 0, 0, "room8");
    room[moster_pos].content = -1;
    room[princess_pos].content = 1;
    /* the map is shown as follow
    ┌────┬────┬────┐
    │ rm6  rm7│ rm8│
    ├────┼    ┼    ┤
    │ rm3  rm4  rm5│
    ├    ┼────┼────┤
    │lobby rm1 rm2 │
    └────┴────┴────┘
    */
    //cout<<"princess "<<princess_pos<<" monster "<<moster_pos<<endl;
}
int main()
{
	ifstream fin; 
	fin.open("in.txt");
    Init();
    string cmd;
    
    while(1)
    {
    	
        if(player.state == -1) //meet the monster
        {
            cout<<"The monster is in the room! You died."<<endl<<"Game over."<<endl;
            break;
        }
        if(player.state == 1 && player.position == 0) //take the princess to the lobby
        {
            cout<<"You take the princess out of the castle!"<<endl<<"You win."<<endl;
            break;
        }
        if(room[player.position].content == 1)
        {
            cout<<"The princess is in the room! Take her out of the castle."<<endl;
        }
        room[player.position].PrintInfo();
        getline(fin, cmd);
        if(cmd == "#end") break; //use string "#end" to mark the end of the file
        string head = cmd.substr(0, 3);
        
        while(head != "go ")
        {
            cout<<"Invalid command!"<<endl;
            getline(fin, cmd);
            if(cmd == "#end") break;
            head = cmd.substr(0,3);
        }
        
            string dire = cmd.substr(3); //get the direction of the command
            if(dire == "north") //go north
                {
                    if(room[player.position].exits[0]) //there is an exit
                    {
                        player.position = (player.position + 3) % 9;
                    }
                    else //no exit
                    {
                        cout<<"Invalid command"<<endl;
                    }
                } 
            if(dire == "south") //go south
                {
                    if(room[player.position].exits[1])
                    {
                        player.position = (player.position - 3) % 9;
                    }
                    else
                    {
                        cout<<"Invalid command"<<endl;
                    }
                    
                }
            if(dire == "west") //go west
                {
                    if(room[player.position].exits[2])
                    {
                        player.position = (player.position -1) % 9;
                    }
                    else
                    {
                        cout<<"Invalid command"<<endl;
                    }
                }
            if(dire == "east") //go east
                {
                    if(room[player.position].exits[3])
                    {
                        player.position = (player.position +1) % 9;
                    }
                    else
                    {
                        cout<<"Invalid command"<<endl;
                    }
                }                 

        if(player.state == 0 || room[player.position].content == -1) player.state = room[player.position].content; //change the state of the player
    }
    return 0;
}
