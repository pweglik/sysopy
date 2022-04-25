#include "stdio.h"
#include "string.h"
#include "stdlib.h"

int main(int argc, char* argv[]){
    if(argc==2){

        char* arg = argv[1];
        char cmd[100] = "mail | tail -n +2 | head -n -2 | sort -k ";
        strcmp(arg,"data")==0?strcat(cmd,"7"):(strcmp(arg,"nadawca")==0?strcat(cmd,"3"):exit(11));
        strcat(cmd, " > view_mails.txt");
        FILE* mails = popen(cmd, "r");
        pclose(mails);

        FILE* result = fopen("view_mails.txt", "r");
        char buff[1024];
        while(fgets(buff, sizeof(buff), result)!=NULL)
            printf(buff);


    }else if(argc==4){
        char* email = argv[1];
        char* title = argv[2];
        char* text = argv[3];
        char cmd[4096] = "mail -s '";
        strcat(cmd, title);
        strcat(cmd ,"' ");
        strcat(cmd, email);

        FILE* process = popen(cmd, "w");
        fputc('"', process);
        fputs(text, process);
        fputc('"', process);
        pclose(process);

    }else{
        printf("invalid arguments\n");
        exit(12);
    }
    return 0;
}