Tristan Pior
My program was developed on Windows 10 in Microsoft Visual Studio.
To compile my program on the cs1.utdallas.edu server, I used the following command:
	g++ -std=c++1y -o main Main.cpp
To run my program on the cs1.utdallas.edu server, I used the following command:
	./main 4 2 test.in 0.9
Where 4 is the number of states
Where 2 is the number of actions
Where test.in is a file containing the input MDP data in the format outlined in the assignment.
Where 0.9 is the discount factor