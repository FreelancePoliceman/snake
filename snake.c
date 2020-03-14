#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BOARDSIZE 17
int SCOREMULTIPLIER;
int max_y,max_x;
int fruit;
int score;
int map[BOARDSIZE][BOARDSIZE];
enum direction { RIGHT = 0, LEFT, UP, DOWN }; //Right is default when initialized
struct snakehead {
   int y,x;
   enum direction direction; 
   int size_to_grow;
} snakehead;
void display(void) {
   /* Display window */
   int base_y = (max_y/2 - BOARDSIZE/2) - 1;
   int base_x = (max_x/2 - BOARDSIZE/2) - 1; /* Don't worry too much about this.*/
   int whitespace[BOARDSIZE * BOARDSIZE];
   int whitespacecount = 0;
   memset(whitespace,0,sizeof(whitespace));
   for (int i = 0; i < BOARDSIZE; ++i) {
      mvprintw(base_y + i, base_x - 1, "|");
      mvprintw(base_y + i, base_x + BOARDSIZE, "|");
      mvprintw(base_y - 1, base_x + i, "-");
      mvprintw(base_y + BOARDSIZE, base_x + i, "-");
   }
   /* Display snake body */
   for (int i = 0; i < BOARDSIZE; ++i) {
      for (int j = 0; j < BOARDSIZE; ++j) {
         if (map[i][j] > 0) {
            mvprintw(base_y + j, base_x + i, "o");
         }
         else if (fruit == 0) {
            whitespace[whitespacecount] = 100*i + j;
            whitespacecount++;
         }
      }
   }
   /* Display head */
   char c;
   switch (snakehead.direction) {
      case RIGHT:
         c = '>';
      break;
      case LEFT:
         c = '<';
      break;
      case DOWN:
         c = 'v';
      break;
      case UP: 
         c = '^';
      break;
   }
   mvprintw(base_y + snakehead.y,base_x + snakehead.x,"%c",c);
   int a = 0;
   if (fruit == 0) {
      a = rand() % whitespacecount;
      fruit = whitespace[a];
   }
   mvprintw(base_y + fruit%100,base_x + fruit/100,"&");
   mvprintw(base_y - 5,max_x/2 - 2, "Score: %d",score);
   refresh();
}  
void clear_old_snake(void) {
   struct comp {
      int y,x;
      int value;
   } compare;
   compare.value = 0;
   int a;
   for (int i = 0; i < BOARDSIZE; ++i) {
      for (int j = 0; j < BOARDSIZE; ++j) {
         if ((a = map[i][j]) != 0) {
            if (a > compare.value) {
               compare.value = a;
               compare.x = i; compare.y = j;
            }
            ++map[i][j];
         }
      }
   }
   if (compare.value != 0) {
      map[compare.x][compare.y] = 0;
   }
}
void quit() {
   endwin();
   exit(0);
}

/*This function avoids repetition when creating menus */
void boxcreate(int local_x, int local_y, int * x_multiplier, int *y_multiplier) { 
   const int a = max_x-local_x;
   const int b = max_y - local_y;
   *x_multiplier = a/local_x;
   *y_multiplier = b/local_y;

   for(int i = local_x; i < local_x*(*x_multiplier); ++i) {
      for(int j = local_y; j < local_y*(*y_multiplier); ++j) {
         mvprintw(j,i," ");
      }
      mvprintw(local_y-1,i,"-");
      mvprintw(local_y*(*y_multiplier),i,"-");
   }
   for (int i = local_y; i < local_y * (*y_multiplier); ++i) {
      mvprintw(i,local_x-1,"|");
      mvprintw(i,local_x*(*x_multiplier),"|");
   }
}

void losegame(void) {
   cbreak();
   int local_y = (25* max_y) / 100;
   int local_x = (25* max_x) / 100;
   int x_multiplier, y_multiplier;
   boxcreate(local_x,local_y,&x_multiplier,&y_multiplier);
   mvprintw(local_y*y_multiplier/2 + local_y/2 - 1,((local_x*x_multiplier)/2 + local_x/2) - 4 , "YOU LOSE!");
   mvprintw(local_y*y_multiplier-1,local_x,"(R) Replay");
   mvprintw(local_y*y_multiplier-1,(local_x*x_multiplier)-8,"(Q) Quit");
   int c;
   refresh();
NO:   
   c = getch();
   if (c == 'q' || c == 'Q')
      quit();
   else if (c == 'r' || c == 'R')
      return;
   goto NO;
}
int main(void) {
   int c;
   initscr();
   keypad(stdscr,TRUE);
   curs_set(0);
   noecho();
REINIT:  
   SCOREMULTIPLIER = 20;
   halfdelay(2);
   getmaxyx(stdscr,max_y,max_x);
   int x_multiplier,y_multiplier,local_x = 25 * max_x /100, local_y = 25 *max_y/100;
   boxcreate(local_x,local_y,&x_multiplier,&y_multiplier);
   int center_x = (local_x * x_multiplier)/2 + local_x/2;
   int center_y = (local_y * y_multiplier)/2 + local_y/2;
   mvprintw(local_y*y_multiplier-1,local_x*x_multiplier- 13, "*");
   do { //Startup menu
      mvprintw(local_y,center_x - 6, "SNAKE GAME!!");
      mvprintw(center_y, center_x - 4, "(S) Start");
      mvprintw(local_y*y_multiplier-1,local_x +2,"Easy Mode");
      mvprintw(local_y*y_multiplier -1 ,local_x*x_multiplier - 11, "Normal Mode");
      refresh();
      switch (c) {
         case 'q':
         case 'Q':
            quit();
         break;
         case 's':
         case 'S':
            goto BEGIN;
         break;
         case KEY_LEFT: //Easy mode           mvprintw(local_y*y_multiplier-1,local_x*x_multiplier - 13, " ");
            mvprintw(local_y*y_multiplier-1,local_x,"*");
            SCOREMULTIPLIER = 10;
            halfdelay(3);
         break;
         case KEY_RIGHT: //Hard mode
            mvprintw(local_y*y_multiplier-1,local_x*x_multiplier - 13, "*");
            mvprintw(local_y*y_multiplier-1,local_x," ");
            SCOREMULTIPLIER = 20;
            halfdelay(2);
      }

   } while ((c = getch()));
BEGIN:
   fruit = 0; score = 0;
   srand(time(0));
   memset(&snakehead,0,sizeof(snakehead));
   memset(map,0,sizeof(map));
   while(true) {
      clear();
      display();
      c = getch();
      switch (c) {
         case 'q':
         case 'Q':
            quit();
         break;
         /*case 'g':
            snakehead.size_to_grow++; 
         break; */
         case ERR:
         break;
         case KEY_RIGHT: 
            if (snakehead.direction != LEFT)
               snakehead.direction = RIGHT;
         break;
         case KEY_LEFT:
            if (snakehead.direction != RIGHT)
               snakehead.direction = LEFT;
         break;
         case KEY_UP:
                    if (snakehead.direction != DOWN)
               snakehead.direction = UP;
         break;
         case KEY_DOWN:
         if (snakehead.direction != UP)
            snakehead.direction = DOWN;
         break;
      }
      if (snakehead.size_to_grow > 0)
         --snakehead.size_to_grow;
      else
         clear_old_snake();
      map[snakehead.x][snakehead.y] = 1;
      refresh();
      switch (snakehead.direction) {
         case RIGHT:
            if (snakehead.x < BOARDSIZE - 1)
               ++snakehead.x;
            else
               goto LOSE;
         break;
         case LEFT:
            if (snakehead.x > 0)
               --snakehead.x;
            else
               goto LOSE;
         break;
         case UP:
            if (snakehead.y > 0)
               --snakehead.y;
            else
               goto LOSE;
         break;
         case DOWN:
            if (snakehead.y < BOARDSIZE - 1)
               ++snakehead.y;
            else
               goto LOSE;
         break;
      }
      if (snakehead.x == fruit/100 && snakehead.y == fruit % 100) {
               ++snakehead.size_to_grow;
         fruit = 0;
         score = score + SCOREMULTIPLIER;
      }
      else if (map[snakehead.x][snakehead.y] != 0) //If player runs into himself
         goto LOSE;
      if (score == BOARDSIZE * BOARDSIZE * SCOREMULTIPLIER - 3*SCOREMULTIPLIER); //Implement a 'win' message sometime
   }
   return 0;
LOSE:
   losegame();
   goto REINIT;
}
