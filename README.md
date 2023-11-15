# SnakeGame-C-Xlib-
Snake game written in c using xlib

![Screenshot from 2023-11-15 19-33-09](https://github.com/pekka1234/SnakeGame-C-Xlib/assets/62663286/6784c892-8ac4-4ae8-9898-bd6bc1923a41)

## Running

You need to have x11 display server. Probably version x116 is needed also.

### Compiling

Use these extra flags (something might be diferent no your system).

Remember that you will need LinkedList.h to be in the same location.

```
gcc -O2 -o SnakeGame SnakeProt.c -L /usr/X11R6/lib -lX11
```

### Executables

There is SnakeGame executable in this repo, it should also work.


