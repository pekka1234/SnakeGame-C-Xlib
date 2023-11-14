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


int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}





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
    // Omena ei saa tulla madon päälle, koska silloin se katoaa!

    int curome;


    // Puts apple on display
    XSetForeground(dpy, gc, red);
    int ox = (rand_lim(65) + 5) * 10;
    int oy = (rand_lim(65) + 5) * 10;

    curome = 1000000 * (ox + 100) + (oy + 100);


    for(int i = 0; i < 10; i++){
            XDrawLine(dpy, w, gc, ox, oy + i, ox + 10, oy + i);
    }

    XSetForeground(dpy, gc, black);

    // Counts how many pixels has snake moved (for snake lenght)
    int ct = 0;

    int waitur = 5;

    int hmw = 50;

    int points = 0;

    // For the very first line drawn
    insertatbegin(x + 100);
    insertatbegin(y + 100);
    insertatbegin(x + 10 + 100);
    insertatbegin(y + 100);
    
    // Moves snake forever
    while(1){
        // Detects arrow key presses
        int down = is_down(dpy, "Down");
        int up = is_down(dpy, "Up");
        int left = is_down(dpy, "Left");
        int right = is_down(dpy, "Right");

        
        // Sets direction based on keypresses (note: this is run only when keys are pressed)
        if(right != 0 && dir != 2){
            waitur = 1;
        }
        if (left != 0 && dir != 1){
            waitur = 2;
        }
        if(down != 0 && dir != 4){
            waitur = 3;
        }
        if(up != 0 && dir != 3){
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


        // If snake goes over borders, it goes to opposite side of the map
        if(x > 750){
            x = 0;
        }
        if(x < 0){
            x = 750;
        }
        if(y > 750){
            y = 0;
        }
        if(y < 0){
            y = 750;
        }

        // Setting up and clearing window for new snake position
        XFlush(dpy);


        // If snake hits itself
        if(searchlist(1000000 * (x + 100) + (y + 100)) != 0){
            printf("osui\n");
            printf("Pisteesi: %i\n", points);
            XFreeGC(dpy, gc);
        }
        
        // If snake finds apple
        if((1000000 * (x + 100) + (y + 100)) == curome){
            points += 1;

            ox = (rand_lim(65) + 5) * 10;
            oy = (rand_lim(65) + 5) * 10;
            curome = 1000000 * (ox + 100) + (oy + 100);

            XSetForeground(dpy, gc, red);

            for(int i = 0; i < 10; i++){
                XDrawLine(dpy, w, gc, ox, oy + i, ox + 10, oy + i);
            }

            XSetForeground(dpy, gc, black);

            hmw += 10;
        }
        // Adds snake cordinates to list for erasing

        if(dir != 0){
            // First thing is for detecting hits
            insertatbegin(1000000 * (x + 100) + (y + 100));
            insertatbegin(x + 100);
            insertatbegin(y + 100);
            insertatbegin(x + 10 + 100);
            insertatbegin(y + 100);
        }
        
        // Draws 10 lines that are 10 long to x and y coordinates
        for(int i = 0; i < 10; i++){
            XDrawLine(dpy, w, gc, x, y + i, x + 10, y + i);
        }

        // Erases snake by its lenght
        if(hmw == 0){

            // Sets color to white, reverses list to usable order and drawls white lines over snake tail
            XSetForeground(dpy, gc, white);

            reverse();

            for(int i = 0; i < 10; i++){
                XDrawLine(dpy, w, gc, GetNth(0) - 100, GetNth(1) + i - 100, GetNth(2) - 100, GetNth(3) + i - 100);
            }

            // Deleting coordinates from list so that next eraser won't erase same parts
            deleteatbegin();
            deleteatbegin();
            deleteatbegin();
            deleteatbegin();
            deleteatbegin();

            reverse();

            XSetForeground(dpy, gc, black);
        }else{
            if(dir != 0){
                hmw -= 1;
            }
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