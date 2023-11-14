#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <time.h> 
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include "LinkedList.h"


// Basic variables for xlib
Display *dpy;
int screen;
Window w;
GC gc;


// Snake coordinates
int x = 100;
int y = 100;


unsigned long black, white, red, blue, green;


// Linked list stuff





// Returns 0 if key is not down;
// A non-zero integer if it is down.
// targetString should be appropriate for XStringToKeysym.
int is_down(Display *dis, char *targetString)
{
    char keys_return[32] = {0};
    KeySym targetSym = XStringToKeysym(targetString);
    KeyCode targetCode = XKeysymToKeycode(dis, targetSym);

    int targetByte = targetCode / 8;
    int targetBit = targetCode % 8;
    int targetMask = 0x01 << targetBit;
    
    XQueryKeymap(dis, keys_return);
    return keys_return[targetByte] & targetMask;
}


// Color function
unsigned long RGB(int r, int g, int b){
    return b + (g<<8) + (r<<16);
}

// Creating colors
void colors(){
    black = RGB(0, 0, 0);
    white = RGB(255, 255, 255);
    red = RGB(255, 0, 0);
    blue = RGB(0, 0, 255);
    green = RGB(0, 255, 0);
}

// Millisecond sleep
int msleep(long tms)
{
    struct timespec ts;
    int ret;

    if (tms < 0){
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = tms / 1000;
    ts.tv_nsec = (tms % 1000) * 1000000;

    do{
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

// X server setup
void xsetup(){
    // Creating basic dispaly elements
    dpy = XOpenDisplay(0); 
    screen = DefaultScreen(dpy);

    // Creating window and setting properties
    w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 750, 750, 5, black, white);
    XSetStandardProperties(dpy, w, "Snake Game", "Version 0", None, NULL, 0, NULL);

    // Detects keyboard presses
    XSelectInput(dpy, w, ExposureMask | ButtonPressMask | KeyPressMask);

    // For drawing and general graphics
    gc = XCreateGC(dpy, w, 0, 0);
    XSetBackground(dpy, gc, white);
    XSetForeground(dpy, gc, black);

    XClearWindow(dpy, w);
    XMapRaised(dpy, w);

    XFlush(dpy);
}




void snake(){
    // Snake color
    XSetForeground(dpy, gc, black);

    // dir is direction
    int dir = 0;

    // Waitur holds next turn until snake hits point which is dividen by 10 (snake lenght)

    ////////// Muistio //////////
    // Tee mato kääntymiis se että voi tehdä sun 180 astetta kääntymisen hyvin nopealla painalluksella eli talletaa kääntymisen arvot johonkin (kaksi muuttujaa jne.)
    // Snakelen eli madon tietty pituus saattaa olla hankalampi juttu

    // Snakelen eli käärmee liikkuminen idea:
    // Tallenetaan muistiin vanhat x ja y koordinnatit, minne käärmettä piirretään, sitten jonkun ajan päästä eli käärmeen pituuden päästä
    // aletaan piirtämään valkoisia viivoja vanhan käärmeen päälle

    // Counts how many pixels has snake moved (for snake lenght)
    int ct = 0;

    int waitur = 5;


    // For the very first line drawn
    insertatbegin(x);
    insertatbegin(y);
    insertatbegin(x + 10);
    insertatbegin(y);
    
    // Moves snake forever
    while(1){
        // Detects arrow key presses
        int down = is_down(dpy, "Down");
        int up = is_down(dpy, "Up");
        int left = is_down(dpy, "Left");
        int right = is_down(dpy, "Right");

        
        // Sets direction based on keypresses (note: this is run only when keys are pressed)
        if(right != 0){
            waitur = 1;
        }
        if (left != 0){
            waitur = 2;
        }
        if(down != 0){
            waitur = 3;
        }
        if(up != 0){
            waitur = 4;
        }


        // When snake hits tile that is turnable, it turns (it turns the same way it is going is turn waitur hast changed)
        if((waitur == 1 || waitur == 2) && (y % 10 == 0)){
            dir = waitur;
        }
        
        if((waitur == 3 || waitur == 4) && (x % 10 == 0)){
            dir = waitur;
        }
        
        // Moves snake to direction (note: this is run every time)
        if(dir == 1){
            x += 1;
        }
        if (dir == 2){
            x -= 1;
        }
        if(dir == 3){
            y += 1;
        }
        if(dir == 4){
            y -= 1;
        }

        if(dir != 0){
            ct += 1;
        }

        // Setting up and clearing window for new snake position
        XFlush(dpy);

        
        // Adds snake cordinates to list for erasing

        if(dir != 0){
            insertatbegin(x);
            insertatbegin(y);
            insertatbegin(x + 10);
            insertatbegin(y);
        }
        
        // Draws 10 lines that are 10 long to x and y coordinates
        for(int i = 0; i < 10; i++){
            XDrawLine(dpy, w, gc, x, y + i, x + 10, y + i);


        }

        // Erases snake by its lenght
        if(ct > 500){

            // Sets color to white, reverses list to usable order and drawls white lines over snake tail
            XSetForeground(dpy, gc, white);

            reverse();

            for(int i = 0; i < 10; i++){
                XDrawLine(dpy, w, gc, GetNth(0), GetNth(1) + i, GetNth(2), GetNth(3) + i);
            }

            // Deleting coordinates from list so that next eraser won't erase same parts
            deleteatbegin();
            deleteatbegin();
            deleteatbegin();
            deleteatbegin();

            reverse();

            XSetForeground(dpy, gc, black);
        }

        //printList();
        //printf("\n\n\n\n\n");
        // Speed of snake
        msleep(5);
    }

    XFlush(dpy);

}


int main(){
    colors();
    xsetup();
    msleep(50);
    snake();

    sleep(100);

    return 0;
}