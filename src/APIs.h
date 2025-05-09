//  \FILE  : APIs.h
//  \BRIEF : Package your api functions here.
// ----------------------------------------------------------------------------

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
	cout<<"SUCCESS ! call back process_1"<<endl;

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
	cout<<"SUCCESS ! call back process_2"<<endl;
///////////////////////////////////////////////////////////
	return;
}

void process_3(vector<vector<string>> OptArgs)
{
	cout<<"SUCCESS ! call back process_3"<<endl;
	return;
}

void process_4(vector<vector<string>> OptArgs)
{
	cout<<"SUCCESS ! call back process_4"<<endl;
	return;
}

void process_5(vector<vector<string>> OptArgs)
{
	cout<<"SUCCESS ! call back process_5"<<endl;
	return;
}