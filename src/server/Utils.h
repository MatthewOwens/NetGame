#ifndef UTILS_H
#define UTILS_H

// The IP address for the server to listen on
#define SERVERIP "127.0.0.1"

// The TCP/UDP port number for the server to listen on
#define SERVERPORT 5555


// Enumeration to describe the state of the player, placed here to access from the server
enum AnimationState {IDLE, LEFT, RIGHT, JUMP_LEFT, JUMP_RIGHT, JUMP, SWING, SHOOT};

// Print an error message and die, try not to use
void die(const char *message);

// multi-platform error checking
int checkError();
#endif
