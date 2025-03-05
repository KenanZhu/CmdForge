//  \FILE  : APIs.h
//  \BRIEF : Package your api functions here.

#pragma once

#include <vector>
#include <iostream>

using namespace std;

////    API FUNCTION MUST BE THE SAME FORMAT AS BELOW:
////
////    ' void (*) (vector<vector<string>>) '
////
////    OTHER FORMATS ARE NOT ALLOWED.

void process_1(vector<vector<string>> OptArgs)
{
////    SPECIFIC YOUR LOGIC COMPLISH HERE
///////////////////////////////////////////////////////////
	cout<<"callback process_1 by command: '"+OptArgs[0][0]+"'."<<endl;

///////////////////////////////////////////////////////////
////    HERE IS YOUR CODE TO PROCESS '[-opts]' AND '[-args]' 
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
////    HERE IS YOUR CODE OR FUNCTION TO PROCESS BY 
////   '[-opts]' AND '[-args]'
///////////////////////////////////////////////////////////
	return;
}
void process_2(vector<vector<string>> OptArgs)
{
////    SAME AS ABOVE
	cout<<"callback process_2 by command: '"+OptArgs[0][0]+"'."<<endl;
///////////////////////////////////////////////////////////
	return;
}