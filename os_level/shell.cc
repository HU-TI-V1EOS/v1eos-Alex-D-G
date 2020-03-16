#include "shell.hh"
#include <iostream>
#include <fstream>

int main(){
	std::string input;
	char * data;

	std::ifstream file("configuratiebestand", std::ios::ate);
        if(file.is_open()){
                std::streampos size = file.tellg();
                data = new char[size];
		file.seekg (0, std::ios::beg);
                file.read(data, size);
                file.close();
        }
	std::string prompt = data;

	while(true){
		std::cout << prompt;
		std::getline(std::cin, input);
		if (input == "new_file") new_file();
		else if (input == "ls") list();
		else if (input == "src") src();
		else if (input == "find") find();
		else if (input == "seek") seek();
		else if (input == "exit") return 0;
		else if (input == "quit") return 0;
		else if (input == "error") return 1;

		if (std::cin.eof()) return 0; }
	 }

void new_file(){
	std::string file;
	std::string text;
	std::cout << "Geef de naam van uw nieuwe bestand:" << std::endl;
	std::getline(std::cin, file);
	std::cout << "Geef de text die u wilt toevoegen:" << std::endl;
	syscall(SYS_creat, file.c_str(), 0777);
	int fd = syscall(SYS_open, file.c_str(), O_WRONLY, 0777);
	std::getline(std::cin, text);
	while(text != "<EOF>"){
		text += "\n";
		syscall(SYS_write, fd, text.c_str(), text.size());
		std::getline(std::cin, text);
	}
}

void list(){
	int gpid = syscall(SYS_fork);
	if (gpid==0){
		const char * args[] = {"/bin/ls", "-la", NULL, NULL};
		syscall(SYS_execve, args[0], args, NULL);
	}
	else{
		syscall(SYS_wait4, gpid, NULL, NULL, NULL);
	}
}

void find(){
	std::string target;
        std::cout << "Geef het woord waar u op wilt filteren:" << std::endl;
        std::getline(std::cin, target);

	int desc[2];
	syscall(SYS_pipe, &desc);

	int fpid = syscall(SYS_fork);
	if (fpid==0){
		syscall(SYS_close, desc[0]);
		const char * args[] = {"/usr/bin/find",  ".", NULL};
		syscall(SYS_dup2, desc[1], 1);
		syscall(SYS_execve, args[0], args, NULL);
	}
	else{
		int cpid = syscall(SYS_fork);
		if(cpid==0){
			syscall(SYS_close, desc[1]);
			const char * args[] = {"/bin/grep", target.c_str(), NULL, NULL};
			syscall(SYS_dup2, desc[0], 0);
			syscall(SYS_execve, args[0], args, NULL);
		}
		else{
			syscall(SYS_close, desc[0]);
			syscall(SYS_close, desc[1]);
			syscall(SYS_wait4, fpid, NULL, NULL, NULL);
			syscall(SYS_wait4, cpid, NULL, NULL, NULL);
		}
	}
}

void seek(){ // ToDo: Implementeer volgens specificatie.
        syscall(SYS_creat, "seek.txt", 0777);
        int fd = syscall(SYS_open, "seek.txt", O_WRONLY, 0777);
        syscall(SYS_write, fd, "x", 1);
	syscall(SYS_lseek, fd, 5000000, SEEK_CUR);
	syscall(SYS_write, fd, "x", 1);

        syscall(SYS_creat, "loop.txt", 0777);
        fd = syscall(SYS_open, "loop.txt", O_WRONLY, 0777);
	syscall(SYS_write, fd, "x", 1);
	for(unsigned int i=0;i<5000000;i++){
		syscall(SYS_write, fd, "\0", 1);
	}
	syscall(SYS_write, fd, "x", 1);
}

void src() // Voorbeeld: Gebruikt SYS_open en SYS_read om de source van de shell (shell.cc) te printen.
{ int fd = syscall(SYS_open, "shell.cc", O_RDONLY, 0755); // Gebruik de SYS_open call om een bestand te openen.
  char byte[1];                                           // 0755 zorgt dat het bestand de juiste rechten krijgt (leesbaar is).
  while(syscall(SYS_read, fd, byte, 1))                   // Blijf SYS_read herhalen tot het bestand geheel gelezen is,
    std::cout << byte; }                                  //   zet de gelezen byte in "byte" zodat deze geschreven kan worden.
