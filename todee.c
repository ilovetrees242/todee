#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#define MARGIN 3

char *tasks[40] = { NULL };

void refreshTT(int *num){
    int numBuf = 0;
    for(int i = 0; i < 40; i++){
        if (tasks[i] == NULL){
            *num = numBuf;
            break;
        }
        numBuf++;
    }
    *num = numBuf;
}
int main(){
    int row,col;
    int ch = 0;
    int inputCh = 0;
    int totalTasks = 0; 
    char taskName[40] = "";
    char filePath[256] = "/home/LFS/.todee/tasks";
    char buffer[40][40];
    for(int i = 0; i < 40; i++){
        for(int j = 0; j < 40; j++){
            buffer[i][j] = '\0';
        }
    }

    setlocale(LC_ALL, "");
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    getmaxyx(stdscr, row, col);
    curs_set(0);
    FILE *taskFile = fopen(filePath, "r");
    if (taskFile == NULL){
        endwin();
        fprintf(stderr, "Error: Failed to open tasks file\n");
        return 1;
    }
    for(int i = 0; i < 40; i++){
        fgets(buffer[i], sizeof(buffer[i]), taskFile);
        if (buffer[i][0] == '\0')
            break;
        tasks[i] = buffer[i];
    }
    fclose(taskFile);
    if(col < 101){
        printw("Error: Required columns: 101 or above 101\n");
        printw("Total columns: %d", col);
        getch();
        endwin();
        return 1;
    }
    WINDOW *win = newwin(row, 60, 0, 42);
    keypad(win, TRUE);
    refreshTT(&totalTasks);
    for(int i = 0; i < 40; i++){
        if (tasks[i] == NULL)
            break;
        mvwprintw(win, i + 1, 1, "%s",tasks[i]);
    }
    box(win, 0, 0);
    mvwprintw(win, 0, 30 - 2.5, "Tasks");
    mvwprintw(win, 0, 3, "%d/%d", totalTasks, sizeof(tasks) / sizeof(tasks[0]));
    attron(A_STANDOUT | A_BOLD);
    for(int y = 0; y < row; y++)
        for(int x = 0; x < 41; x++) 
            mvwprintw(stdscr, y, x, " ");
    mvwprintw(stdscr, 1, MARGIN, "Press N to create a new task");
    mvwprintw(stdscr, 2, MARGIN, "Press D to delete the selected task");
    mvwprintw(stdscr, 3, MARGIN, "Press E to edit the selected task");
    mvwprintw(stdscr, 4, MARGIN, "Press Q to exit");
    mvwprintw(stdscr, 5, MARGIN, "Press C to clear everything");
    mvwprintw(stdscr, 6, 0, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    attroff(A_STANDOUT | A_BOLD);
    refresh();

    while(ch = wgetch(win)){
        switch(ch){
            case 'N':
                int lineCount = MARGIN + 12;
                int count = 8;
                int bufferCount = 0;
                attron(A_BOLD | A_STANDOUT);
                mvwprintw(stdscr, 7, MARGIN, "Creating a task");
                mvwprintw(stdscr, 8, MARGIN, "Enter name: ");
                memset(taskName, '\0', 40);
                curs_set(1); echo();
                while(1){
                    if (lineCount > 41 - MARGIN){
                        count++; 
                        lineCount = MARGIN;
                    }
                    inputCh = mvgetch(count, lineCount);
                    if (inputCh == '\b' || inputCh == KEY_BACKSPACE){
                        if (count == 8){
                            if (lineCount > MARGIN + 12)
                                lineCount--;
                        }
                        else if(lineCount > MARGIN)
                            lineCount--;
                        else if (lineCount == MARGIN){  
                            count--;
                            lineCount = 41 - MARGIN;
                        }
                        mvaddch(count, lineCount, ' ');
                        taskName[strlen(taskName) - 1] = '\0';
                        bufferCount--;
                        continue;
                    }
                    else if(inputCh == KEY_LEFT || inputCh == KEY_RIGHT || inputCh == KEY_UP || inputCh == KEY_DOWN)
                        continue;
                    if (inputCh == '\n' || inputCh == 27)
                        break;
                    if ( bufferCount == 40 ){
                        mvaddch(count, lineCount, ' ');
                        continue;
                    }
                    else {
                        taskName[strlen(taskName)] = inputCh;
                        lineCount++;
                        bufferCount++;
                        refresh();
                    }
                }
                if(taskName[0] == '\0'){
                    mvwprintw(stdscr, row - 2, MARGIN, "Task name cannot be empty!");
                    noecho();
                    curs_set(0);
                    attroff(A_BOLD | A_STANDOUT);
                    refresh();
                    break;
                }
                for(int i = 0; i < sizeof(tasks) / sizeof(tasks[0]); i++){
                    if ( tasks[i] == NULL ){
                        tasks[i] = taskName;
                        mvwprintw(win, i + 1, 1, "%s", tasks[i]);
                        FILE *fp = fopen("/home/LFS/.todee/tasks", "a");
                        if (fp == NULL){
                            endwin();
                            fprintf(stderr, "Failed to open tasks file\n");
                            return 1;
                        }
                        fputs(tasks[i], fp);
                        fputs("\n", fp);
                        break;
                        fclose(fp);
                    }
                }
                curs_set(0); noecho();
                for(int i = 0; i < 41; i++){
                    mvwprintw(stdscr, 7, i, " ");
                    mvwprintw(stdscr, 8, i, " ");
                    mvwprintw(stdscr, 9, i, " ");
                    mvwprintw(stdscr, row - 2, i, " ");
                }
                attroff(A_BOLD | A_STANDOUT);
                refreshTT(&totalTasks);
                mvwprintw(win, 0, 3, "%d/%d", totalTasks, sizeof(tasks) / sizeof(tasks[0]));
                refresh();
                break;
            case 'Q':
                endwin();
                exit(0);
                break;
            case KEY_DOWN:
                endwin();
                exit(0);
                break;
            case KEY_UP:
                endwin();
                exit(0);
                break;
            case 'C':
                FILE *taskFile = fopen(filePath, "w");
                if (taskFile == NULL){
                    endwin();
                    fprintf(stderr, "Failed to open the tasks file");
                    return 1;
                }
                fclose(taskFile);
                memset(tasks, 0, sizeof(tasks));
                for(int y = 0; y < row - 2; y++)
                    for(int x = 0; x < 58; x++)
                        mvwprintw(win, y + 1, x + 1, " ");
                refreshTT(&totalTasks);
                mvwprintw(win, 0, 3, "%d/%d", totalTasks, sizeof(tasks) / sizeof(tasks[0]));
                refresh();
                break;
            default:
                printw("%d\n", ch);
                refresh();
        }
    }
    endwin();
}
