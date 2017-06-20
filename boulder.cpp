#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <cassert>
#include <algorithm>

using namespace std;
typedef pair<int,int> pii;
#define MATRIX_SIZE 40
class state;


pii map_size, from, to;
vector<state> registered_states;
map<state, int> reg_state_map;
vector<int> prev_state;

bool matrix[MATRIX_SIZE][MATRIX_SIZE];

const int dir_y[4] = {1, -1, 0, 0};
const int dir_x[4] = {0, 0, 1, -1};


class state{
public:
	vector<pii> boulders;
	pii pos;	

	state(vector<pii> _boulders, pii _pos) : 
	boulders(_boulders), 
	pos(_pos){
	}

	bool boulder_at(pii p){
		for(unsigned int i=0; i<boulders.size(); i++)
			if(boulders[i] == p) 
				return true;		
		return false;
	}

	void move_boulder(pii from, pii to){
		for(unsigned int i=0; i<boulders.size(); i++){
			if(boulders[i] == from){
				boulders[i] = to;				
				return;
			}
		}
	}

	friend bool operator== (const state &s1, const state &s2){
		return s1.boulders == s2.boulders && s1.pos == s2.pos;
	}

	friend bool operator<(const state& s1, const state& s2){
		if(s1.pos < s2.pos)
			return true;
		if(s1.pos > s2.pos)
			return false;
		return s1.boulders < s2.boulders;
	}
};

bool inside_matrix(pii p){
	if(p.first < 0 || p.second < 0) return false;
	if(p.first >= map_size.first) return false;
	if(p.second >= map_size.second) return false;
	return true;
}

bool wall_at(pii p){
	return matrix[p.first][p.second];
}

bool state_is_registered(state s){
	auto it = reg_state_map.find(s);
	if(it != reg_state_map.end()){
		return true;
	}
	return false;
}

void register_state(state s, int id_prev){
	reg_state_map[s] = registered_states.size(); // ID of this state
	registered_states.push_back(s); // Position in vector = ID of this state
	prev_state.push_back(id_prev); // ID of previous	
}

int state_id(state s){

	auto it = reg_state_map.find(s);
	if(it != reg_state_map.end()){
		return it->second;
	}
	return -1;
}


bool check_solution(vector<string> solution, state first_state){

	pii new_pos;
	pii new_boulder_pos;

	for(auto dir : solution){

		pii pos = first_state.pos;

		int d = -1;

		if(dir == "←"){
			d = 3;
		} else if(dir == "→"){
			d = 2;
		} else if(dir == "↑"){
			d = 1;
		} else if(dir == "↓"){
			d = 0;
		}

		if(d == -1) return false;

		new_pos = make_pair(pos.first+dir_y[d], pos.second+dir_x[d]);
		new_boulder_pos = make_pair(new_pos.first+dir_y[d], new_pos.second+dir_x[d]);

		if(!inside_matrix(new_pos)) return false;
		if(wall_at(new_pos)) return false;
		if(first_state.boulder_at(new_pos)){
			// there's a boulder
			if(!inside_matrix(new_boulder_pos)) return false;
			if(wall_at(new_boulder_pos)) return false;
		}
		first_state.move_boulder(new_pos, new_boulder_pos);
		first_state.pos = new_pos;
	}

	return first_state.pos == to;
}



int main(){

	int a, b;

	cin >> a >> b;
	map_size = make_pair(a, b);

	vector<pii> initial_boulders;

	for(int i=0; i<map_size.first; i++){
		for(int j=0; j<map_size.second; j++){
			char c;
			cin >> c;
			c = toupper(c);
			switch(c){
				case 'X':
				matrix[i][j] = true;
				break;
				case 'O':
				initial_boulders.push_back(make_pair(i, j));
				break;
				case 'A':
				from = make_pair(i, j);
				break;
				case 'B':
				to = make_pair(i, j);
				break;
				default:
				matrix[i][j] = false;
			}
		}
	}

	state init(initial_boulders, from);

	queue<state> states;
	states.push(init);
	register_state(init, -1);

	while(!states.empty()){
				
		state current = states.front(); 
		states.pop();

		// Get solution
		if(current.pos == to){

			int id = state_id(current);

			vector<string> dirs;

			while(id != -1){
				if(prev_state[id] != -1){
					pii current_pos = registered_states[id].pos;				
					pii prev_pos = registered_states[prev_state[id]].pos;
					if(current_pos.second < prev_pos.second) dirs.push_back("←");						
					else if(current_pos.second > prev_pos.second) dirs.push_back("→");
					else if(current_pos.first > prev_pos.first) dirs.push_back("↓");
					else dirs.push_back("↑");
				}				
				id = prev_state[id];
			}

			reverse(dirs.begin(), dirs.end());
			assert(check_solution(dirs, init));

			for(auto s : dirs){
				cout << s << endl;
			}

			return 0;
		}

		// Find more states
		for(int d=0; d<4; d++){

			pii new_pos = make_pair(
				current.pos.first + dir_y[d], 
				current.pos.second + dir_x[d]
				);

			if(!inside_matrix(new_pos) || matrix[new_pos.first][new_pos.second]){
				// off boundaries
				// or
				// can't walk into position
				continue;
			}

			if(current.boulder_at(new_pos)){
				// move boulder
				pii new_boulder_pos = make_pair(
					new_pos.first + dir_y[d], 
					new_pos.second + dir_x[d]
					);

				if(inside_matrix(new_boulder_pos) && 
					!matrix[new_boulder_pos.first][new_boulder_pos.second] &&
					!current.boulder_at(new_boulder_pos)){
					// boulder can't go off boundaries
					// boulder can't move here
					// boulder can't move if there's another boulder there

					state new_state(current.boulders, new_pos);
					new_state.move_boulder(new_pos, new_boulder_pos);

					if(!state_is_registered(new_state)){
						states.push(new_state);
						register_state(new_state, state_id(current));
					}
				}
					
			} else {
				// there's no boulder. Can move here
				state new_state(current.boulders, new_pos);

				if(!state_is_registered(new_state)){
					states.push(new_state);
					register_state(new_state, state_id(current));
				}
			}
		}
	}
	
	cout << "no" << endl;	

	return 0;
}
