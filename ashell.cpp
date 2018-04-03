#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>  
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <dirent.h>
#include <termios.h>
#include <ctype.h>
#include <vector>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void ffhelper(char *dirname, char *filename){
    DIR *myffdir;
	struct dirent *myffstruct;
	struct stat f_details;
	if(dirname !=NULL){//to make sure you input ff filename dir
	   chdir(dirname);
	  }
	
	myffdir = opendir(".");//pointing at the current dir
	while((myffstruct = readdir(myffdir)) != NULL){
    //check if its a directory
        if((myffstruct->d_type == DT_DIR) && ((strcmp(myffstruct-> d_name,".") !=0) && (strcmp(myffstruct->d_name, "..")!=0))){ 
            ffhelper(myffstruct->d_name, filename);
        }
        
        if(strcmp(myffstruct->d_name,filename)==0){
                if(dirname!=NULL){//to write if the dir isnt null
                    write(1,"./",sizeof("./"));
                    write(1,dirname,strlen(dirname));
                }
                write(1,"/", sizeof("/"));
                write(1, myffstruct->d_name, strlen(myffstruct->d_name));
                write(1, "\n", sizeof("\n"));

        }
	}//while
	closedir(myffdir);
	if(dirname != NULL){
	chdir("..");	
	}
}

//outputs the current workding directory path
void mypwd(){
    char wd_buffer[1024], wd_buffer1[1024];

	char* mypwd;		
	mypwd = getcwd(wd_buffer,sizeof(wd_buffer));
	write(1,mypwd,strlen(mypwd));
    write(1,"\n", 1);
				
}


void myls(char *path){ 

    char *mycd = new char[10000];
    char *mydata = new char;
    char *temp;
    char *mypwd, *myprompt;
    char wd_buffer[1024], wd_buffer1[1024];
    DIR *mydir,*myffdir;
    struct stat mystart;
    struct dirent *mydirstruct,*myffstruct;
    string sub_data;
    struct termios SavedTermAttributes;
    char RXChar;
    int i =0,j=0;
    int itemp;
    char currentinput[1024],testinput[1024];
    vector<string> recordinput;
    string tempstring;
    int historynum=0;
    string tempPrompt;
    int pCount = 0;
    string finalP;
    char *token3[256];

    int lsCount = 0;
    if(path == '\0'){//if its just ls
        mydir=opendir(getcwd(wd_buffer,sizeof(wd_buffer))); 
    } else{
          chdir(path);
          mydir=opendir(getcwd(wd_buffer,sizeof(wd_buffer))); 
    }
    while((mydirstruct = readdir(mydir)) != NULL){      //citation: http://pubs.opengroup.org/onlinepubs/009695399/functions/readdir.html
        
        if((!(stat(mydirstruct->d_name, &mystart) == 0)) && (!(stat(mydirstruct->d_name, &mystart)> 0))){
            //return 1;
            return;
        }
        if(strcmp(mydirstruct->d_name,".")!=0 && strcmp(mydirstruct->d_name,"..")!=0){
                write(1, (S_ISDIR(mystart.st_mode)) ? "d" : "-", 1);
                write(1, (mystart.st_mode & S_IRUSR) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWUSR) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXUSR) ? "x" : "-", 1);
                write(1, (mystart.st_mode & S_IRGRP) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWGRP) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXGRP) ? "x" : "-", 1);
                write(1, (mystart.st_mode & S_IROTH) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWOTH) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXOTH) ? "x" : "-", 1);
                write(1, " ",1);
                write(1,mydirstruct->d_name,strlen(mydirstruct->d_name)); 
                write(1,"\n", 1);
        }else{
                lsCount++;
                write(1, (S_ISDIR(mystart.st_mode)) ? "d" : "-", 1);
                write(1, (mystart.st_mode & S_IRUSR) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWUSR) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXUSR) ? "x" : "-", 1);
                write(1, (mystart.st_mode & S_IRGRP) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWGRP) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXGRP) ? "x" : "-", 1);
                write(1, (mystart.st_mode & S_IROTH) ? "r" : "-", 1);
                write(1, (mystart.st_mode & S_IWOTH) ? "w" : "-", 1);
                write(1, (mystart.st_mode & S_IXOTH) ? "x" : "-", 1);
                write(1, " ",1);
                if(lsCount == 1){
                    write(1,".",sizeof("."));
                }else{ 
                    if(lsCount == 2){
                    write(1,"..",sizeof(".."));
                    }
                }
                write(1,"\n",sizeof("\n"));
            }
    }//while
    closedir(mydir);
    if(path != '\0'){
        chdir("..");
    }
        
    tempstring="";
    for (j=0;j<=i;j++){
        currentinput[j]='\0';
    }
    i=0;			
}//end of ls

void executestuff(char commands[]){
    char * mycommand=commands;
    char *token[256];
    char **token2[256];
    char mytoken[100][1024];
    int action[100]; // 3 to pipe previous to next, 1 to redirect 
    int j=0,i=0,numactions=0;
    int finalredirect=-1,numpipes=-1,finalpipe=-1;
    int noPipe = -1, oneDirect = -1;
    pid_t pid;
    while(*mycommand!='\0'){ 
        if(*mycommand==' '){
            if(i!=0 && mytoken[numactions][i-1]==' '){ //there is already a space written
                //do nothing
            } else if(i!=0){
                mytoken[numactions][i]=' '; //if there isn't already a space between the works add it.
                i++;
            }
        } else if(*mycommand=='|'){ //pipe detect
            mytoken[numactions][i]='\0';
            noPipe++;
            finalpipe=numactions;
            action[numactions]=3;
            numactions++;
            numpipes++;
            i=0;

        } else if (*mycommand=='>'){//redirect detect
            mytoken[numactions][i]='\0';
            oneDirect++;
            action[numactions]=1;
            finalredirect=numactions;
            i=0;
            numactions++;
        }  else if (*mycommand=='<'){//file read detect
            mytoken[numactions][i]='\0';
            action[numactions]=2;
            i=0;
            numactions++;
        } else { //we have a letter
            mytoken[numactions][i]=*mycommand; //append to token array the letter
            i++;
        }
            
        mycommand++;	

    } //end while parsing loop

    mytoken[numactions][i]=*mycommand;	

    int allpipes[numpipes][2];

    for(i=0;i<=numpipes;i++){ //create all pipes
        if(pipe(allpipes[i])!=0){
            write(1,"pipe error",sizeof("pipe error"));
        }
    }

    i=0;
    int numchildren=numactions;
    if(finalredirect!=-1){
        numchildren--;
    }

    for(i=0;i<=numchildren;i++){ //fork repipe execute 
            int mypipe[2];//unsused
            pipe(mypipe);//usused
            j=0;
            token[j] = strtok(mytoken[i], " "); //tokenize the current instruction
        
            while(token[j] != NULL){
                j++;
                token[j] = strtok(NULL, " ");
            }



        pid =fork();
        if(pid==0){ //child process
            if (action[i]==1 && i==finalredirect){ //redirect to file
                FILE* file = fopen(mytoken[i+1], "w");
                dup2(fileno(file), fileno(stdout)); //direct stdout to the file, we are ready to execute
                fclose(file);
                if(i!=0){ //not first child need to read from previous pipe
                    dup2(allpipes[i-1][0],0);
                }

            } else if (action[i]==2){//read from file works for simple cases like cat < filename
                int in = open(mytoken[i+1], O_RDONLY);
                dup2(in, 0);
                close(in);

            } else if(action[i]==3){ //need to setup pipe
                if(i!=numchildren){ //if not last child
                    dup2(allpipes[i][1],1); //write to current pipe
                } if(i==0){ // if first child
                    close(allpipes[i][0]);//close read end of pipe
                    for (int t=0;t<=numpipes;t++){ // close all unused pipes
                        if(t!=i){
                            close(allpipes[t][0]);
                            close(allpipes[t][1]);
                        }

                    }
                }
            } //end if action[i]==3
            if(i!=0){ // if i is not first child and not last child
                    dup2(allpipes[i-1][0],0);//read from previous pipe

                for (int t=0;t<=numpipes;t++){ // close all unused pipes
                    if(t!=(i-1)&& t!=i){		//dont close the current used pipe pipe
                        close(allpipes[t][0]);
                        close(allpipes[t][1]);
                    }

                }
                close(allpipes[i-1][1]); //close write end of previous pipe
                close(allpipes[i][0]);	//close read end of current pipe
            }		
            //write(1,"2",1);
            if(strncmp(token[0],"ff",2)==0){
                ffhelper(token[2],token[1]);//pass directory, filename it's reversed
                exit(0);
            } else if(strncmp(token[0],"pwd",3)==0){
                mypwd();
                exit(0);
            } else if(strncmp(token[0],"ls",2)==0){
                myls(token[1]); 
                exit(0);
            } else{
            execvp(token[0],token);
            }
        } else if (pid>0){
            for (int t=0;t<=numpipes;t++){
                if (t!=numpipes){
                    
                }
                close(allpipes[t][0]);
                close(allpipes[t][1]);

            }
            wait(NULL);
            
        }
    }
} // end executestuff


void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void SetNonCanonicalMode(int fd, struct termios *savedattributes){
    struct termios TermAttributes;
    char *name;
    
    // Make sure stdin is a terminal. 
    if(!isatty(fd)){
        fprintf (stderr, "Not a terminal.\n");
        exit(0);
    }
    
    // Save the terminal attributes so we can restore them later. 
    tcgetattr(fd, savedattributes);
    
    // Set the funny terminal modes. 
    tcgetattr (fd, &TermAttributes);
    TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO. 
    TermAttributes.c_cc[VMIN] = 1;
    TermAttributes.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}


int main(int argc, char *argv[]){
    char *mycd = new char[10000];
    char *mydata = new char;
    char *temp;
    char *mypwd, *myprompt;
    char wd_buffer[1024], wd_buffer1[1024];
    DIR *mydir,*myffdir;
    struct stat mystart;
    struct dirent *mydirstruct,*myffstruct;
    string sub_data;
    struct termios SavedTermAttributes;
    char RXChar;
    int i =0,j=0;
    int itemp;
    char currentinput[1024],testinput[1024];
    vector<string> recordinput;
    string tempstring;
    int historynum=0;
    string tempPrompt;
    int pCount = 0;
    string finalP;
    char *token3[256];

    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);

    myprompt = getcwd(wd_buffer1,sizeof(wd_buffer1));//need this when it detects enter and first time when code compiles
        tempPrompt = string(myprompt);
        if(strlen(myprompt) <= 16){
            write(1,myprompt, strlen(myprompt));
            write (1, "%", sizeof("%"));
                    write(1, " ", sizeof(" "));
        }
        else{
                 write (1, "/.../", sizeof("/.../"));
                reverse(tempPrompt.begin(),tempPrompt.end());//in algorithm library
                while(tempPrompt[pCount] != '/'){
                    finalP = tempPrompt[pCount] + finalP;
                    pCount++;
                }
            
                write(1, finalP.c_str(), finalP.length());
                write (1, "%", sizeof("%"));
                write(1, " ", sizeof(" "));
        }

    while (true){
        read(STDIN_FILENO, &RXChar, 1);
            if(0x04 == RXChar){ // C-d
                break;
            } else if (isprint(RXChar)){
            write(1,&RXChar,1);
            currentinput[i]=RXChar;
            i++;
        } else if(RXChar==0x7F){ //backspace
            if(i>0){
                write(1,"\b \b",sizeof("\b \b"));
                currentinput[i]=0x0A;
                i--;//decrement index to ovverride next position

            } else {
                write(1,"\a",sizeof("\a"));
            }
    //detect arrow key		
        } else if (RXChar==0x1B){ //detect arrow key
            //arrowhandle(recordinput,i,&historynum);
            char tempchar,tempchar1;
            string temp;
            int size=recordinput.size();
            read(0,&tempchar,1);
            if(tempchar==0x5B){//detect left bracket character
                read(0,&tempchar1,1);
                if(tempchar1==0x42){//down arrow
                    if(historynum>0){ //make sure we stay in allotted space
                        historynum --; //we want to look at the previous command
                        //delete current
                        for(j=0;j<i;j++){
                            write(1,"\b \b",sizeof("\b \b"));
                        }
                        temp=recordinput[size-1-historynum];
                        //paste most recent command
                        strcpy(currentinput,temp.c_str());
                        i=strlen(currentinput);
                        j=strlen(temp.c_str());
                        write(1,temp.c_str(),j);
                        
                    } else {
                        write(1,"\a",sizeof("\a"));
                    }
        
                } else if (tempchar1==0x41){//up arrow
                    if(historynum<size){
                        //delete current
                        for(j=0;j<i;j++){
                            write(1,"\b \b",sizeof("\b \b"));
                        }
                        temp=recordinput[size-1-historynum];
                        //paste most recent command
                        strcpy(currentinput,temp.c_str());
                        i=strlen(currentinput);
                        j=strlen(temp.c_str());
                        write(1,temp.c_str(),j);
                        historynum++;
                    } else {
                        write(1,"\a",sizeof("\a"));
                    }
                }
            }
        
        } else if(RXChar==0x0A){ //detect enter
            //checking input is correct
            //add to histroy vector
            currentinput[i]='\0';
            write(1,"\n",sizeof("\n"));
            recordinput.push_back(currentinput);
            if(strcmp(currentinput,"exit")==0){
                //write(1,"\n", 1);
                break;
            } else if(strncmp(currentinput,"cd",2)==0){  
                    
                if(strlen(currentinput) == 2 || (strcmp(currentinput,"cd home")==0))
                {
                chdir(getenv("HOME"));
                }
                    else{
                            size_t pos = (string(currentinput).find(" "));
                            sub_data = (string(currentinput).substr(pos));
                            sub_data.erase(0,1); //to erase space
                            const char* path = sub_data.c_str(); //c_str used to convert from string char*
                            if(chdir(path)==0){
                                chdir(path);
                                                
                            }else{
                                write(1,"unable to set directory",sizeof("unable to set directory"));
                                write(1,"\n",sizeof("\n"));
                            } 
                }//else		    
                //erase temps
                tempstring="";
                for (j=0;j<=i;j++){
                    currentinput[j]='\0';
                }	
                i=0;
            }else {  //fork and exec +ls, pwd, ff
                executestuff(currentinput);
                //erase temps
                tempstring="'\0'";
                
                for (j=0;j<1024;j++){
                    currentinput[j]='\0';
                }//for	
                i=0;
                j=0;
            } 
        
        myprompt = getcwd(wd_buffer1,sizeof(wd_buffer1));//need this when it detects enter and first time when code compiles
        tempPrompt = string(myprompt);
        if(strlen(myprompt) <= 16){
            write(1,myprompt, strlen(myprompt));
            write (1, "%", sizeof("%"));
                    write(1, " ", sizeof(" "));
        }

        else{
                write (1, "/.../", sizeof("/.../"));
                reverse(tempPrompt.begin(),tempPrompt.end());//in algorith library
                while(tempPrompt[pCount] != '/'){
                    finalP = tempPrompt[pCount] + finalP;
                    pCount++;
                 }
        
                write(1, finalP.c_str(), finalP.length());
                write (1, "%", sizeof("%"));
                write(1, " ", sizeof(" "));
                //write(1, "\n", sizeof("\n"));
        }
                tempstring="'\0'";
                for (j=0;j<1024;j++){
                    currentinput[j]='\0';
                }//for	
                i=0;
                j=0;
        }//detect enter
    }//end while loop
    ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    return 0;
} // end main





