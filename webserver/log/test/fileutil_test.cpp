#include"fileutil.h"


int main(){
	AppendFile appendfile("new_log.txt");
	//char * buf="adasdsadasdadasda";
	appendfile.append("lvke111",9);
	appendfile.flush();

}
