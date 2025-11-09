#include "system_info.h"
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

void initColors() {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);     // Header background
    init_pair(2, COLOR_WHITE, COLOR_BLUE);      // System info window
    init_pair(3, COLOR_WHITE, COLOR_BLACK);     // Default text
    init_pair(4, COLOR_BLACK, COLOR_CYAN);      // Selection highlight
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);    // Status messages
}

void drawHeader(WINDOW* win, int width) {
    wbkgd(win, COLOR_PAIR(1));
    werase(win);
    mvwprintw(win, 0, (width - 19) / 2, "System Monitor Tool");
    wrefresh(win);
}

void displaySystemInfo(WINDOW* win) {
    wbkgd(win, COLOR_PAIR(2));
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " System Info ");

    CPUData cpu = readCPUStats();
    MemoryInfo mem = readMemoryInfo();
    double uptime = getUptime();

    mvwprintw(win, 2, 2, "Uptime: %.2f hours", uptime / 3600.0);
    mvwprintw(win, 3, 2, "Memory: %ld MB / %ld MB",
              (mem.totalMem - mem.availableMem) / 1024, mem.totalMem / 1024);
    wrefresh(win);
}

void displayProcessList(WINDOW* win, const std::vector<ProcessInfo>& processes, int selectedRow) {
    wbkgd(win, COLOR_PAIR(3));
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Processes ");

    int maxRows, maxCols;
    getmaxyx(win, maxRows, maxCols);

    mvwprintw(win, 1, 2, "PID    USER      STATE  MEM(KB) MEM%%  COMMAND");

    int rowsToShow = maxRows - 3;
    for (int i = 0; i < rowsToShow && i < (int)processes.size(); ++i) {
        if (i == selectedRow)
            wattron(win, COLOR_PAIR(4) | A_BOLD);

        mvwprintw(win, i + 2, 2, "%-6d %-9s %-6c %-7lu %5.2f %s",
                  processes[i].pid,
                  processes[i].user.c_str(),
                  processes[i].state,
                  processes[i].memUsage,
                  processes[i].memPercent,
                  processes[i].command.c_str());

        if (i == selectedRow)
            wattroff(win, COLOR_PAIR(4) | A_BOLD);
    }

    wrefresh(win);
}

void displayStatus(WINDOW* win, const char* message) {
    wbkgd(win, COLOR_PAIR(5));
    werase(win);
    mvwprintw(win, 0, 2, message);
    wrefresh(win);
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    if (!has_colors()) {
        endwin();
        printf("Terminal does not support colors.\n");
        return 1;
    }
    initColors();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    WINDOW* headerWin = newwin(1, cols, 0, 0);
    WINDOW* sysWin = newwin(5, cols / 3, 1, 0);
    WINDOW* procWin = newwin(rows - 7, cols, 6, 0);
    WINDOW* statusWin = newwin(1, cols, rows - 1, 0);

    int sortMode = 2;
    int selectedRow = 0;

    displayStatus(statusWin, "Sort: P-ID C-PU M-emory | Move: Up/Down | Kill: K | Quit: Q");

    while (true) {
        drawHeader(headerWin, cols);
        displaySystemInfo(sysWin);

        std::vector<int> pids = getProcessPIDs();
        std::vector<ProcessInfo> processes;
        MemoryInfo mem = readMemoryInfo();

        for (int pid : pids) {
            ProcessInfo proc = readProcessInfo(pid);
            proc.user = getProcessUser(pid);
            proc.memPercent = (mem.totalMem > 0) ? (100.0 * proc.memUsage / mem.totalMem) : 0;
            processes.push_back(proc);
        }
        sortProcesses(processes, sortMode);
        displayProcessList(procWin, processes, selectedRow);

        int ch = getch();
        switch (ch) {
            case 'p': case 'P': sortMode = 0; break;
            case 'c': case 'C': sortMode = 1; break;
            case 'm': case 'M': sortMode = 2; break;
            case KEY_UP: if (selectedRow > 0) selectedRow--; break;
            case KEY_DOWN: if (selectedRow < (int)processes.size()-1) selectedRow++; break;
            case 'k': case 'K':
                if (selectedRow >= 0 && selectedRow < (int)processes.size()) {
                    kill(processes[selectedRow].pid, SIGTERM);
                    displayStatus(statusWin, "Kill signal sent.");
                }
                break;
            case 'q': case 'Q':
                delwin(headerWin);
                delwin(sysWin);
                delwin(procWin);
                delwin(statusWin);
                endwin();
                return 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
