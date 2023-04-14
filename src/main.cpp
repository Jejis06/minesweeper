#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <map>
#include <utility>

#ifdef __unix__    
	#include <termios.h>
	#include <unistd.h>

	#define CLEAR system("clear");

	int getch(void)
	{
		struct termios oldattr, newattr;
		int ch;
		tcgetattr( STDIN_FILENO, &oldattr );
		newattr = oldattr;
		newattr.c_lflag &= ~( ICANON | ECHO );
		tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
		ch = getchar();
		tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
		return ch;
	}

#elif defined(_WIN32) || defined(WIN32)    
    #include <conio.h>
	#include <ctime>
	#define CLEAR system("cls");

#endif

using namespace std;

//windows stupid


class Board {
		
	//TODO: 
	//menu
	
	public:
		vector<pair<int,int>> combs(int y, int x){
			vector<pair<int,int>> combinations = {
				{y-1,x-1},
				{y-1,x},
				{y-1,x+1},

				{y,x-1},
				{y,x+1},

				{y+1,x-1},
				{y+1,x},
				{y+1,x+1},
			};
			return combinations;

		}
		string load_format(string base, int num){
			int end,it=1,lim;
			string format = base;
			while(num > 0){
				end = num%10;
				lim = format.size()-it;
				if (lim < 0){
					it = 1;
					lim = format.size()-it;
				}
				format[lim] = char(48 + end);
				num /= 10;
				it++;
			}
			return format;
		}
		void saper_bar(){

			cout << START << color_frame;
			cout <<sep <<frame_char << sep;
			for(int jj=bounds;jj<=w;jj++)
				cout <<sep <<ver_frame_char << sep;
			cout <<sep <<frame_char << sep;
			cout << '\n';
				
			cout <<sep <<frame_char << sep;
			cout << END;
			
			//MAIIN BAR
			
			//mvs;
			//flags;
			
			int MIN = bounds-1+(2+sep.size()); 	
			int MAX = (w+1)*(2+sep.size());

			int SIZE = MAX;

			string bar = format_mines + load_format(format_number,dens-flags);
			
			if(lost) won = false; 

			if (won) bar = "   You won!";
			if (lost) bar = "   You lost :(";

			int BREAK_LINE = int(SIZE/2)-int(bar.size()/2);

			cout << START << color_bar;
			for(int i=MIN;i<MAX;i++){
				if (i == BREAK_LINE){ 
					cout << bar;
					i+= bar.size();
				}
				cout << " ";	
			}

			cout << START << color_frame;
			cout <<sep <<frame_char << sep;
			cout <<'\n';
			cout <<sep <<frame_char << sep;
			for(int jj=bounds;jj<=w;jj++)
				cout <<sep <<ver_frame_char << sep;
				
			cout <<sep <<frame_char << sep;
			cout << END << '\n';
		}

		void process_move(char in){
			if(CONTROLS.count(in) == 0) return;
			short cmd = CONTROLS.at(in);

			if (cmd == LEFT){// Left
				cursor_x = (cursor_x - 1) > 0?(cursor_x - 1):1;
				return;
			}
			else if (cmd == DOWN){// Down
				cursor_y = (cursor_y + 1) > h?h:(cursor_y + 1);
				return;
			}
			else if (cmd == UP){// Up
				cursor_y = (cursor_y - 1) > 0?(cursor_y - 1):1;
				return;
			}
			else if (cmd == RIGHT){// Right
				cursor_x = (cursor_x + 1) > w?w:(cursor_x + 1);
				return;
			}
			else if (cmd == FLAG){// flag 
				if (mask[cursor_y][cursor_x] == 2){
					mask[cursor_y][cursor_x] = 0;
					flags--;
				}	
				else if (mask[cursor_y][cursor_x] == 0){
					flags++;	
					mask[cursor_y][cursor_x] = 2; 	
				}
			}
			else if (cmd == CHECK){// reveal
				if (first_mv){
					mines();
					setall();
					first_mv = false;
				}
				reveal(cursor_y,cursor_x);
			}
			if (freed == w*h-dens) won = true;

		}
		void reveal(int y, int x){
			if(mask[y][x]==1) return;
			
			if (mask[y][x] == 2) flags--;

			if(map[y][x] == -1){
				allmines();
				lost = true;
			}
			else magic_helper(y,x);

		}
		void magic_helper(int y, int x,int swap=0 , bool gen=0){
			if (mask[y][x] == 1) return;

			if (mask[y][x] == 2) flags--;
			mask[y][x] = 1;
			freed++;

			if (map[y][x] == 0){
				auto combinations = combs(y,x);
				for(auto cm : combinations){
					if (cm.first >= 1 && cm.second >= 1 && cm.first <= h && cm.second <= w)
						magic_helper(cm.first,cm.second,swap+1,1);
				}
				
			}
			else if(map[y][x] != -1){
				auto combinations = combs(y,x);
				for(auto cm : combinations){
					if(swap > w*h/dens) break;
					if (cm.first >= 1 && cm.second >= 1 && cm.first <= h && cm.second <= w){
						if(map[cm.first][cm.second] == -1 ){
							if(gen) break;
							else continue;
						}
						magic_helper(cm.first,cm.second,swap+1,1);
					}
				}

			}


		}
		void Input(){
		
			cursor_x = w/2;
			cursor_y = h/2;

			char cmd;
			while(!lost && !won){
				cmd = getch();
				process_move(cmd);
				Frame();
								
			}
			Frame();
		}
		//visuals
		void Frame(){
			CLEAR

			if (won && !lost){
				if(freed == 0) complete_board(); 
				allmines();
				mine = flag;
				color_bomb = color_flag;
			}

			for(int i=bounds;i<h+bounds;i++){
				if(i==bounds){
					cout << START << color_frame;
					for(int h=0;h<3;h++){
						if(h==1){

							cout <<sep <<frame_char << sep;
							cout << sep;

							for(int jj=bounds+2;jj<=(w+1)*(2+sep.size())-3;jj++){
								if(jj == (w+1)*(2+sep.size())/2 - int(title.size()/2)){
									cout << END << START << color_title << title << END << START << color_frame;
									jj += title.size(); 
									continue;
								}
								cout << sep ;
									
							}
							cout <<sep<<sep;
							cout <<sep <<frame_char << sep << '\n';
						}
						cout <<sep <<frame_char << sep;
						for(int jj=bounds;jj<=w;jj++)
							cout <<sep <<ver_frame_char << sep;
						cout <<sep <<frame_char << sep;
						cout << '\n';
					}
					
					cout << END;
				}
				for(int j=bounds;j<w+bounds;j++){
					if (j==bounds) cout << START << color_frame << sep << frame_char << sep <<END;
					cout <<START; 
					if(i == cursor_y && j == cursor_x) cout << color_cursor;
					if(mask[i][j] == 1) {
						if (map[i][j] == -2 || map[i][j] == 0) cout << color_empty << sep<< empty<<sep; 
						else if(map[i][j] == -1) cout <<color_bomb << sep << mine << sep; 
						else cout << color_num_dep[map[i][j]-1]<< color_num << sep << map[i][j] << sep;
					}
					else if(mask[i][j] == 2){
						cout << color_flag << sep<< flag << sep;
					}
					else cout<< color_background << sep << hidden << sep ;	
					cout << END;
				}
				cout << START << color_frame << sep << frame_char << sep<<END;
				cout << '\n';

			}	
			saper_bar();
		}
		void allmines(){
			while(MINES.size()){
				t = MINES.top();
				mask[t.first][t.second] = 1;
				MINES.pop();
			}
		}			
		void complete_board(){
			for(int i= bounds;i<h+bounds;i++){
				for(int j=bounds;j<w+bounds;j++){
					map[i][j] = -1;
					MINES.push({i,j});
				}
			}
		}

		int calculate_val(int y, int x){
			int val=0;
			if(map[y][x] == -1) return -1;
			auto combinations = combs(y,x);
			for(int i=0;i<8;i++){
				pair<int,int> t = combinations[i];
				if (map[t.first][t.second] == -1) val++;
			}
			return val;
		}
		void setall(){
			for(int i=bounds;i<h+bounds;i++){
				for(int j=bounds;j<w+bounds;j++){
					map[i][j] = calculate_val(i,j);	
				}
			}	

		}
		void mines(){
			vector <pair<int,int>> visited;

			srand(time(0));

			if (dens < w*h) visited.push_back({cursor_y,cursor_x});

			for(int i=0;i<dens;i++){
				bool skip = false;
				if(visited.size() == w*h) break;

				int x = rand()%w + 1;
				int y = rand()%h + 1;

				
				for(int j=0;j<visited.size();j++){
					t = (visited[j]);
					if(t.first == y && t.second == x){
						i--;
						skip = true;
					}
					
				}
				if (skip) continue;
				
				MINES.push({y,x});
				visited.push_back({y,x});
				map[y][x] = -1;

			}
		}
		void init(){
			mask = new short* [h+2*bounds];
			map = new short* [h+2*bounds];

			for(int i=0;i<h+2*bounds;i++){
				map[i] = new short[w+bounds*2];
				mask[i] = new short[w+bounds*2];
			}

			for(int i=bounds;i<h+bounds;i++){
				for(int j=bounds;j<w+bounds;j++){
					map[i][j] = -2;
					mask[i][j] = 0;
				}
			}	

			
		}
		//constructor destructor
		Board(int w, int h, int dens, int prc=false): 
			w(w), h(h), dens(dens)
		{
			if (prc) dens = int(float((h*w)/100 * dens) * 100);
			init();
		}
		~Board(){
			delete[] map;
		}

	private:
		int bounds=1;
		int w,h;
		int dens;
		short** map;
		short** mask;
		bool** selected_mask;
		bool lost=false;
		bool won = false;
		stack<pair<int,int>> MINES;
		int cursor_x, cursor_y;
		int freed=0;
		int flags=0;
		int mvs=0;

		bool first_mv=true;
		pair<int,int> t;//temp

		short LEFT =	1,
		      DOWN =	2,
		      UP =	3,
		      RIGHT =	4,
		      FLAG =	5,
		      CHECK =	6;

		::map<char,short> CONTROLS = {
			{'h',LEFT},
			{'j',DOWN},
			{'k',UP},
			{'l',RIGHT},

			{'a',LEFT},
			{'s',DOWN},
			{'w',UP},
			{'d',RIGHT},

			{'r',CHECK},
			{'f',FLAG}

		};

		//style
		
		string sep = " ";

		string hidden = "_";	
		string mine = "*";
		string empty = "#";
		string flag = "!";
		string frame_char = "|";
		string ver_frame_char="-";

		string title="Minesweeper";
		
		string START = "\e[";
		string END = "\e[0m";
		
		string format_number = "0000";
		string format_mines = "* : ";

		string color_title="30;100m";//100m	
		string color_bar="91;40m";
		string color_frame_bg = "100m";//43m - yellow
		string color_frame="30;" + color_frame_bg;
		string color_flag= "30;45m";
		string color_empty = "37;40m";
		string color_bomb = "91;6;40m";
		string color_cursor = "47";
		string color_num = ";40m";
		string color_background = "30;40m";
		string color_num_dep[8] = {
			"34",
			"32",
			"31",
			"35",
			"33",
			"36",
			"96",
			"95",
		};
};

int main(){
	int w,h,d;

	cout << "Saper\n";
	cout << "recomended settings w:20 h:20 b:160 | w:10 h:10 b:20\n";
	cout << "width : ";
	cin >> w;
	cout << "height : ";
	cin >> h;
	cout << "bombs : ";
	cin >> d;
	// nice lvls 10x10/30 | 20x20/120 | 10x20/70
	Board brd(w,h,d);
//	brd.allmines();
	brd.Input();
}
