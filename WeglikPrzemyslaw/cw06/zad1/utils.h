#ifndef UTILS_H
#define UTILS_H

#define MAX_CLIENTS  10
#define PROJECT_ID 0x099
#define MAX_CONT_SIZE 4096

typedef enum mtype {
    INIT = 1, LIST = 2, MSG2ALL = 3, MSG2ONE = 4, STOP = 5
} mtype;

typedef struct Message {
    long mtype;
    pid_t sender_pid;
    char message_text[MAX_CONT_SIZE];
} Message;

// msgsz does not contain mtype
const size_t MSG_SIZE = sizeof(Message) - sizeof(long);

#endif