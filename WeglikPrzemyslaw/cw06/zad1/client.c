#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>

#include "utils.h"

#define FAILURE_EXIT(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }

void register_client(key_t privateKey);
void request_list(Message *msg);
void request_msg2all(Message *msg);
void request_msg2one(Message *msg);
void request_stop(Message *msg);
int create_queue(char*, int);
void close_queue();
void int_handler(int);

int sessionID = -2;
int queue_descriptor = -1;
int privateID = -1;


int main() {
    if(atexit(close_queue) == -1)
        FAILURE_EXIT("Registering client's atexit failed");
    if(signal(SIGINT, int_handler) == SIG_ERR)
        FAILURE_EXIT("Registering INT failed");

    char* path = getenv("HOME");
    if (path == NULL)
        FAILURE_EXIT("Getting $HOME failed");

    queue_descriptor = create_queue(path, PROJECT_ID);

    key_t privateKey = ftok(path, getpid());
    if (privateKey == -1)
        FAILURE_EXIT("Generation of private key failed");

    privateID = msgget(privateKey, IPC_CREAT | IPC_EXCL | 0666);
    if (privateID == -1)
        FAILURE_EXIT("Creation of private queue failed");

    register_client(privateKey);

    char cmd[20];
    Message msg;
    while(1) {
        msg.sender_pid = getpid();
        printf("client: enter your request: ");
        if (fgets(cmd, 20, stdin) == NULL){
            printf("client: error reading your command\n");
            continue;
        }
        int n = strlen(cmd);
        if (cmd[n-1] == '\n') cmd[n-1] = 0;


        if (strcmp(cmd, "list") == 0) {
            request_list(&msg);
        } else if (strcmp(cmd, "msg2all") == 0) {
            request_msg2all(&msg);
        } else if (strcmp(cmd, "msg2one") == 0) {
            request_msg2one(&msg);
        } else if (strcmp(cmd, "stop") == 0) {
            request_stop(&msg);
        } else {
            printf("client: incorrect command\n");
        }
    }
}

void register_client(key_t privateKey) {
    Message msg;
    msg.mtype = INIT;
    msg.sender_pid = getpid();
    sprintf(msg.message_text, "%d", privateKey);

    if (msgsnd(queue_descriptor, &msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("client: INIT request failed\n");
    if (msgrcv(privateID, &msg, MSG_SIZE, 0, 0) == -1)
        FAILURE_EXIT("client: catching INIT response failed\n");
    if (sscanf(msg.message_text, "%d", &sessionID) < 1)
        FAILURE_EXIT("client: scanning INIT response failed\n");
    if (sessionID < 0)
        FAILURE_EXIT("client: server cannot have more clients\n");

    printf("client: client registered. Session no: %d\n", sessionID);
}

// Message handlers

void request_list(Message *msg) {
    msg->mtype = LIST;

    if (msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("client: LIST request failed");
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
        FAILURE_EXIT("client: catching LIST response failed");
    printf("%s", msg->message_text);
}

void request_msg2all(Message *msg) {
    msg->mtype = MSG2ALL;
    printf("Enter message to send: ");
    if (fgets(msg->message_text, MAX_CONT_SIZE, stdin) == 0) {
        printf("client: too many characters\n");
        return;
    }
    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("client: MSG2ALL request failed");
    if(msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
        FAILURE_EXIT("client: catching MSG2ALL response failed");
    printf("%s", msg->message_text);
}

void request_msg2one(Message *msg) {
    msg->mtype = MSG2ONE;
    printf("Enter other client's queue ID: ");
    if (fgets(msg->message_text, MAX_CONT_SIZE, stdin) == 0) {
        printf("client: too many characters\n");
        return;
    }
    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("client: MSG2ONE request failed");
    if(msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
        FAILURE_EXIT("client: catching MSG2ONE response failed");
    printf("%s\n", msg->message_text);
}

void request_stop(Message *msg) {
    msg->mtype = STOP;

    if(msgsnd(queue_descriptor, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("client: STOP request failed");
}

// utility functions

int create_queue(char *path, int ID) {
    int key = ftok(path, ID);
    if(key == -1) FAILURE_EXIT("Generation of key failed");

    int QueueID = msgget(key, 0);
    if (QueueID == -1) FAILURE_EXIT("Opening queue failed");

    return QueueID;
}

void close_queue() {
    if (privateID > -1) {
        if (msgctl(privateID, IPC_RMID, NULL) == -1){
            printf("There was some error deleting clients's queue\n");
        }
        else {
            printf("Client's queue deleted successfully\n");
        }
    }
}

void int_handler(int _) { 
    Message*  msg;
    request_stop(msg);
    exit(2); 
}