/*	Tristan Pior
	CS4375.002
	A3P1
	MDP w/ Bellman's
	Due Date: 10/11/21
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

#define TIME_DEPTH 20

using namespace std;

class Action {
	public: vector<vector<double>> p;

			//Default Constructor
	public: Action() {
		//Do Nothing
	}

			//Constructor
	public: Action(int numStates) {
		//Set the size of the matrix to numStates x numStates
		//numStates is given from the command line argument for number of states
		for (int i = 0; i < numStates; i++) {
			vector<double> a;
			for (int j = 0; j < numStates; j++) {
				a.push_back(0.0);
			}
			p.push_back(a);
		}
	}

};



class MDP {
	private: vector<double> states;			//doubles contained represent the reward value for each state
	private: vector<Action> actions;		//contains all actions which each contain their own 2d probability matrix
	private: vector<vector<double>> jTable;	//dynamic programming table used in value iteration
	private: double discount;				//discount factor used in Bellman's equation

			 //Constructor
	public: MDP(int numStates, int numActions, string fileName, double d) {
		//Populate actions vector
		for (int i = 0; i < numActions; i++) {
			Action a(numStates);
			actions.push_back(a);
		}
		discount = d;				//Set the discount factor (input from command line arg)
		for (int i = 0; i < numStates; i++) {
			vector<double> a;
			jTable.push_back(a);
		}
		input(fileName, numStates);			//Input the file
	}

			//Grabs from the input file line by line
			//Each line is tokenized . . .
			//1st is state and will match i in the loop
			//2nd is reward for that state and is placed into states array
			//3rd+ is actions for that state and the chances are placed into the 
			//	appropriate positions in the action's probability matrix (currentState, nextState)
	private: void input(string fileName, int numStates) {
		ifstream read(fileName);	//Input stream to read from file
		string line;	//Holds the input line from read
		string token;	//Holds the token from line

		stringstream ss;

		//Input all lines in input file
		while (getline(read, line)) {
			ss.str("");	//Reset ss to accept a new string
			ss.clear();
			ss << line;	//Input next line

			vector<string> lineTok;

			ss >> token;	//State name
			int currStateIndex = stoi(token.substr(1)) - 1;
			ss >> token;	//State reward value
			double r = stod(token);
			states.push_back(r);

			//Parse line and populate lineTok vector with tokens
			//Rest of ss contains the actions of the current state
			ss >> token;
			while (!ss.eof()) {
				//ss >> token;	//Action name with leading "("
				int actionIndex = stoi(token.substr(2)) - 1;
				ss >> token;	//Target state of action
				int targetStateIndex = stoi(token.substr(1)) - 1;
				ss >> token;	//Chance of this action with trailing ")"
				int endingParen = token.find(")");
				double chance = stod(token.substr(0, endingParen));
				actions[actionIndex].p[currStateIndex][targetStateIndex] = chance;
				ss >> token;
			}
		}
	}

			 //Computes the optimal policy using value iteration with Bellman's equation
			 //Uses and maintains dynamic programming table, jTable
			 //Calls the print function at each time step as per assignment description
	public: void computePolicy() {
		//Set base case
		for (size_t i = 0; i < states.size(); i++) {
			jTable[i].push_back(states[i]);
		}
		//Print time step k = 1
		cout << "After iteration 1:" << endl;
		printPolicy(0);
		for (int k = 1; k < TIME_DEPTH; k++) {
			//Set inductive case for time step k
			for (size_t i = 0; i < states.size(); i++) {
				jTable[i].push_back(states[i] + (discount * actionMax(i, k)));
			}
			//Print time step k + 1
			cout << "After iteration " << (k + 1) << ":" << endl;
			printPolicy(k);
		}
	}

			//Used to compute the second half of Bellman's equation
			//(the max of all actions available at that state)
			//s is the index tied to a particular state
			//k is the time step
	private: double actionMax(int s, int k) {
		double currMax = numeric_limits<double>::lowest();	//set to an impossibly low value (minimum double value)
		//Iterate through all actions
		for (size_t i = 0; i < actions.size(); i++) {
			double tempMax = 0;
			Action *a = &actions[i];
			bool valid = false;
			for (size_t j = 0; j < a->p[s].size(); j++) {
				if (a->p[s][j] != 0) { valid = true; }
				tempMax += a->p[s][j] * jTable[j][k - 1];
			}
			//If the newly calculated max is greater, then set that to the new max
			if (tempMax > currMax && valid) { currMax = tempMax; }
		}
		return currMax;
	}

			 //Used to compute the argmax for finding the best action in state s
			 //Returns the index of the max action
			 //s is the index tied to a particular state
			 //k is the time step of the J* values
	private: int actionArgMax(int s, int k) {
		double currMax = numeric_limits<double>::lowest();	//set to an impossibly low value (minimum double value)
		int maxIndex = -1;
		//Iterate through all actions
		for (size_t i = 0; i < actions.size(); i++) {
			double tempMax = 0;
			Action *a = &actions[i];
			for (size_t j = 0; j < states.size(); j++) {
				tempMax += a->p[s][j] * jTable[j][k];
			}
			//If the newly calculated max is greater, then set that to the new max
			if (tempMax > currMax) {
				currMax = tempMax;
				maxIndex = i;
			}
		}
		return maxIndex;
	}

			 //Prints the policy with the "J*" values at the current iteration
			 //k is the time step that contains the J* values
	private: void printPolicy(int k) {
		cout << fixed;	//Set cout to fixed floating point notation
		cout << setprecision(4);
		for (size_t i = 0; i < jTable.size(); i++) {
			cout << "(s" << (i + 1) << " a" << (actionArgMax(i, k) + 1) << " " << jTable[i][k] << ") ";
		}
		cout << endl;
	}

};

int main(int argc, char** argv) {
	//If there is an improper number of arguments, then terminate.
	if (argc != 5) {
		cout << "Invalid number of arguments. Requires 1. number of states, 2. number of actions, 3. input file, 4. discount factor." << endl;
	}

	MDP vi(stoi(argv[1]), stoi(argv[2]), argv[3], stod(argv[4]));
	vi.computePolicy();
}