#include <iostream>
#include <string>

std::string translate(std::string line, std::string argument){
	std::string result = "";
	int index=0;
	while(index==0){
		size_t area = argument.find(line);
		if(area!=std::string::npos){
			argument.erase(area, line.length());
		}
		else{
			index=1;
		}
	}
	result = argument;
	return result;
}

int main(int argc, char *argv[]){
	std::string line;

	if(argc != 2){
		std::cerr << "Deze functie heeft exact 1 argument nodig" << std::endl;
		return -1;
	}

	while(std::getline(std::cin, line)){
		std::cout << translate(line, argv[1]) << std::endl;
	}

  	return 0;
 }
