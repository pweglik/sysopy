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

void handle_public_queue(struct Message *msg);
void do_login(struct Message *msg);
void do_mirror(struct Message *msg);
void do_calc(struct Message *msg);
void do_time(struct Message *msg);
void do_end(struct Message *msg);
int find_queue_id(pid_t sender_pid);
int create_message(struct Message *msg);
char* convert_time(const time_t *mtime);
void close_queue();
void int_handler(int);

int queue_descriptor = -2;
int active = 1;
int clients_data[MAX_CLIENTS][2];
int client_count = 0;

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
    if (atexit(close_queue) == -1)
        FAILURE_EXIT("server: egistering server's atexit failed\n");

    if (signal(SIGINT, int_handler) == SIG_ERR)
        FAILURE_EXIT("server: registering INT failed\n");

    struct msqid_ds current_state;

    char* path = getenv("HOME");
    if(path == NULL)
        FAILURE_EXIT("server: getting environmental variable 'HOME' failed\n");

    key_t publicKey = ftok(path, PROJECT_ID);
    if(publicKey == -1)
        FAILURE_EXIT("server: generation of publicKey failed\n");

    queue_descriptor = msgget(publicKey, IPC_CREAT | IPC_EXCL | 0666);
    if(queue_descriptor == -1)
        FAILURE_EXIT("server: creation of public queue failed\n");

    Message buffer;
    while(1) {
        if (active == 0) {
            if (msgctl(queue_descriptor, IPC_STAT, &current_state) == -1)
                FAILURE_EXIT("server: getting current state of public queue failed\n");
            if (current_state.msg_qnum == 0) break;
        }

        if (msgrcv(queue_descriptor, &buffer, MSG_SIZE, 0, 0) < 0)
            FAILURE_EXIT("server: receiving message failed\n");
        handle_public_queue(&buffer);
    }
    return 0;
}

void handle_public_queue(struct Message *msg) {
    if (msg == NULL) return;
    switch(msg->mtype){
        case INIT:
            do_init(msg);
            break;
        case LIST:
            do_list(msg);
            break;
        case MSG2ALL:
            do_msg2all(msg);
            break;
        case MSG2ONE:
            do_msg2one(msg);
            break;
        default:
            break;
    }
}

// HANDLERS ////////////////////////////////////////////////////////////////////

void do_init(struct Message *msg) {
    key_t client_queue_key;
    if (sscanf(msg->message_text, "%d", &client_queue_key) < 0)
        FAILURE_EXIT("server: reading client_queue_key failed\n");

    int client_queue_id = msgget(client_queue_key, 0);
    if (client_queue_id == -1)
        FAILURE_EXIT("server: reading client_queue_id failed\n");

    int client_pid = msg->sender_pid;
    msg->mtype = INIT;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("server: maximum number of clients reached\n");
        sprintf(msg->message_text, "%d", -1);
    } else {
        clients_data[client_count][0] = client_pid;
        clients_data[client_count++][1] = client_queue_id;
        sprintf(msg->message_text, "%d", client_count - 1);
    }

    if (msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("server: INIT response failed\n");
}

void do_list(Message *msg) {
    printf("server: LIST received\n");
    printf("CLIENTS:\n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients_data[i][0] != -1)
            printf("\tid: %d    queue_id: %d\n", clients_data[i][0], clients_data[i][1]);
    }
}

void do_msg2all(Message *msg) {
    printf("server: MSG2ALL received\n");
    for(int i=0; i<MAX_CLIENTS; i++){
        if (clients_data[i][0] != -1)
        {
            Message* msg_to_send;
            msg_to_send->mtype = MSG2ONE;
            msg_to_send->sender_pid = getpid();

            time_t timer;
            time(&timer);
            char* timeStr = convert_time(&timer);

            strcpy(msg_to_send->message_text, msg->message_text);
            free(timeStr);

            if(msgsnd(clients_data[i][1], msg, MSG_SIZE, 0) == -1)
                FAILURE_EXIT("server: MSG2ONE response failed\n");
            }
    }
}


void do_msg2one(struct Message *msg) {
    printf("server: MSG2ONE received\n");
    int client_queue_id = atoi(msg->message_text);
    if(client_queue_id == -1) return;

    Message* msg_to_send;
    msg_to_send->mtype = MSG2ONE;
    msg_to_send->sender_pid = getpid();

    time_t timer;
    time(&timer);
    char* timeStr = convert_time(&timer);

    strcpy(msg_to_send->message_text, "Hello single receiver!");
    free(timeStr);

    if(msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1)
        FAILURE_EXIT("server: MSG2ONE response failed\n");
}

// HELPERS /////////////////////////////////////////////////////////////////////

int create_message(struct Message *msg) {
    int client_queue_id = find_queue_id(msg->sender_pid);
    if (client_queue_id == -1){
        printf("server: client not found\n");
        return -1;
    }

    msg->mtype = msg->sender_pid;
    msg->sender_pid = getpid();

    return client_queue_id;
}

int find_queue_id(pid_t sender_pid) {
    for (int i=0; i < MAX_CLIENTS; ++i) {
        if(clients_data[i][0] == sender_pid)
            return clients_data[i][1];
    }
    return -1;
}

char* convert_time(const time_t *mtime) {
    char* buff = malloc(sizeof(char) * 30);
    struct tm * timeinfo;
    timeinfo = localtime (mtime);
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    return buff;
}

void close_queue() {
    if (queue_descriptor > -1) {
        int tmp = msgctl(queue_descriptor, IPC_RMID, NULL);
        if (tmp == -1) {
            printf("server: there was some error deleting server's queue\n");
        }
        printf("server: queue deleted successfully\n");
    }
}

void int_handler(int _) { exit(2); }