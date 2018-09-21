

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

struct splitted_str
 {
      int n_lines;
      char** str;
 };

splitted_str str_split(char* s, unsigned int max_len=100)
{
	std::string str(s);
	
	unsigned int n_lines = (unsigned int)std::ceil(((double)str.length())/max_len);

	//allocate the array
	char** ret_str = new char*[n_lines];
	
	for(int i = 0; i < n_lines; i++)
		ret_str[i] = new char[max_len+5];

	for (unsigned int i = 0; i < n_lines; i++) {
		strcpy(ret_str[i], str.substr(i*max_len, max_len).c_str());
		strcat(ret_str[i], "\r\n");
	}
	
	//for (unsigned int i = 0; i < n_lines; i++) {
	//	std::cout<< ret[i]<<std::endl;
	//}
	
	splitted_str ret;
	ret.n_lines = n_lines;
	ret.str = ret_str;
	
	return ret;
}