// Goat! Goat is a program that displays an annoying goat that is
// jungry every 10 seconds and wont stop screaming until you feed it 
// and is sleepy 10 seconds after that, to which you can get it to sleep and will sleep 
// for 10 seconds more 
// Author: Sofia Bolanos
// Date completed: Nov 21, 2026

//----To compile and run---------
/* gcc hackathon.c -o hackathon -lncurses
 ./hackathon goat_map.txt */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <time.h>
#include <errno.h>

#define FRAME_RATE 6

#define WIDTH 30
#define HEIGHT 30
#define SIZE (WIDTH * HEIGHT)

// Time settings (in seconds)
#define HUNGER_TIME 5
#define SLEEPY_TIME 10
#define SLEEP_DURATION 5

// Map Goat
#define WALL 'W'
#define EMPTY_SPACE 'H'
#define FLOOR 'E'
#define GOAT 'G'
#define HORN 'T'
#define EYE 'O'
#define MUZZLE 'M'
#define BODY 'A'
#define HOOVES 'K'
#define BED_FRAME 'D'
#define BLANKET 'B'
#define PILLOW 'P'
#define CLOCK 'C'

// Custom color IDs
#define LIGHT_BROWN_COLOR 8
#define MEDIUM_BROWN_COLOR 9
#define DARK_BROWN_COLOR 10

// Color pair IDs
#define PAIR_LIGHT_BROWN 12
#define PAIR_MEDIUM_BROWN 13
#define PAIR_DARK_BROWN 14

// Every cell corresponds to a color
typedef enum
{
    BROWN,
    BLUE,
    GREEN,
    RED,
    PURPLE,
    PINK,
    YELLOW,
    GRAY,
    WHITE,
    BLACK,
    BBBLUE,
    LIGHT_BROWN,
    MEDIUM_BROWN,
    DARK_BROWN
} Cell;

// Struct to keep track of the goat's status
typedef struct{
    bool hungry;
    bool sleepy;
    bool sleeping;
    time_t last_fed;
    time_t last_awake;
    time_t sleep_end;
} Goat;

// Takes a row and column and returns the exact index in our array
int get_index(int row, int col)
{
    return row * WIDTH + col;
}

/**
 * Sleep for a given number of milliseconds
 * \param ms Number of milliseconds to sleep for
 */
void sleep_ms(size_t ms)
{
    struct timespec ts;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;

    // If nanosleep is interrupted, continue sleeping for the remaining time
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
    {
    }
}

// Converts each character from the map file into a Cell
Cell char_to_cell(char c){
    switch (c)
    {
        case EMPTY_SPACE:
            return GRAY;
        case FLOOR:
            return BROWN;
        case GOAT:
            return MEDIUM_BROWN;
        case BODY:
            return LIGHT_BROWN;
        case HORN:
            return DARK_BROWN;
        case EYE:
            return BLACK;
        case MUZZLE:
            return PINK;
        case HOOVES:
            return DARK_BROWN;
        case BLANKET:
            return BLUE;
        case PILLOW:
            return WHITE;
        case BED_FRAME:
            return DARK_BROWN;
        case CLOCK:
            return YELLOW;
        case WALL:
            return BBBLUE;
        default:
            return GRAY;
    }
}

// Initializes all color pairs used by the map
void initialize_colors(void)
{
    start_color();

    // Original color pairs
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);
    init_pair(6, COLOR_BLACK, COLOR_GREEN);
    init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(8, COLOR_WHITE, COLOR_MAGENTA);
    
    init_pair(10, COLOR_BLACK, COLOR_CYAN);
    init_pair(11, COLOR_WHITE, COLOR_BLACK);

    // Creating three custom shades of brown

        init_color(LIGHT_BROWN_COLOR, 600, 300, 150);
        init_color(MEDIUM_BROWN_COLOR, 500, 200, 100);
        init_color(DARK_BROWN_COLOR, 350, 120, 60);

        // Light brown
        init_pair(PAIR_LIGHT_BROWN, COLOR_BLACK, LIGHT_BROWN_COLOR);

        // Medium brown
        init_pair(PAIR_MEDIUM_BROWN, COLOR_BLACK, MEDIUM_BROWN_COLOR);

        // Dark brown
        init_pair(PAIR_DARK_BROWN, COLOR_BLACK, DARK_BROWN_COLOR);
    
}

// Returns the color pair associated with each Cell
int get_color_pair(Cell cell)
{
    switch (cell)
    {
        case BLUE:
            return 1;
        case YELLOW:
            return 2;
        case RED:
            return 3;
        case WHITE:
            return 4;
        case GRAY:
            return 5;
        case GREEN:
            return 6;
        case PURPLE:
            return 7;
        case PINK:
            return 8;
        case BROWN:
        case MEDIUM_BROWN:
            return PAIR_MEDIUM_BROWN;
        case BBBLUE:
            return 10;
        case BLACK:
            return 11;
        case LIGHT_BROWN:
            return PAIR_LIGHT_BROWN;
        case DARK_BROWN:
            return PAIR_DARK_BROWN;
        default:
            return 4;
    }
}

// Prints the entire map using the corresponding colors
void print_board(Cell *map, Goat *goat, int frame)
{
    // Go through every row
    for (int row = 0; row < HEIGHT; row++)
    {
        // Go through every column
        for (int col = 0; col < WIDTH; col++)
        {
            int index = get_index(row, col);

            // Get the corresponding color pair
            int pair = get_color_pair(map[index]);

            // Animate the goat's mouth while screaming
            if (map[index] == PINK && goat->hungry)
            {
                if ((frame / 2) % 2 == 0)
                {
                    pair = 11;
                }
            }

            attron(COLOR_PAIR(pair));

            // Draw two spaces to create a square tile
            mvaddstr(row, col * 2, "  ");

            attroff(COLOR_PAIR(pair));
        }
    }

    // Display screaming letters 
    if (goat->hungry)
    {
        attron(COLOR_PAIR(3) | A_BOLD);
        mvprintw(8, 12, "AHHHHH!");
        attroff(COLOR_PAIR(3) | A_BOLD);
    }

    // Display sleeping or sleepy message
    else if (goat->sleeping)
    {
        attron(A_BOLD);
        mvprintw(8, 12, "zzz...");
        attroff(A_BOLD);
    }
    else if (goat->sleepy)
    {
        mvprintw(8, 12, "UGHHH....");
    }

    // Display player instructions
    if (goat->hungry)
    {
        mvprintw(HEIGHT, 0, "GOAT IS HUNGRY! Press F to feed!");
    }
    else if (goat->sleeping)
    {
        mvprintw(HEIGHT, 0, "Goat is sleeping... sweet dreams!");
    }
    else if (goat->sleepy)
    {
        mvprintw(HEIGHT, 0, "Goat is sleepy! Press S to sleep!");
    }
    else
    {
        mvprintw(HEIGHT, 0, "Goat is happy! Q = Quit");
    }

    mvprintw(HEIGHT + 1, 0, "F = Feed | S = Sleep | Q = Quit");

    refresh();
}

// Feeds the goat and stops it from screaming
void feed(Goat *goat)
{
    if (goat->hungry)
    {
        goat->hungry = false;

        // Reset the feeding timer
        goat->last_fed = time(NULL);
    }
}

// Lets the goat sleep for 15 seconds
void goat_sleep(Goat *goat)
{
    if (goat->sleepy && !goat->hungry && !goat->sleeping)
    {
        goat->sleepy = false;
        goat->sleeping = true;

        // Set the time when the goat should wake up
        goat->sleep_end = time(NULL) + SLEEP_DURATION;
    }
}

// Updates the goat's status based on elapsed time
void update_goat(Goat *goat)
{
    time_t current_time = time(NULL);

    // Check if the goat has become hungry
    if (!goat->hungry &&
        difftime(current_time, goat->last_fed) >= HUNGER_TIME)
    {
        goat->hungry = true;
    }

    // Check if the goat has become sleepy
    if (!goat->sleepy && !goat->sleeping &&
        difftime(current_time, goat->last_awake) >= SLEEPY_TIME)
    {
        goat->sleepy = true;
    }

    // Check if the goat's 15 seconds of sleep are over
    if (goat->sleeping && current_time >= goat->sleep_end)
    {
        goat->sleeping = false;

        // Reset the sleeping timer
        goat->last_awake = current_time;
    }
}

// Reads keyboard input and updates the goat's status
void check_input(bool *running, Goat *goat, Cell *map)
{
    (void)map; // Map is not needed for keyboard input

    int input;

    // Handle multiple inputs
    while ((input = getch()) != ERR)
    {
        // Exit when the user types q
        if (input == 'q' || input == 'Q')
        {
            *running = false;
        }
        // Feed the goat
        else if (input == 'f' || input == 'F')
        {
            feed(goat);
        }
        // Let the goat sleep
        else if (input == 's' || input == 'S')
        {
            goat_sleep(goat);
        }
    }
}

// Loads a map from a text file
void load_map(const char *filename, Cell *map)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error opening map file");
        exit(1);
    }

    // Go through every row
    for (int row = 0; row < HEIGHT; row++)
    {
        // Go through every column
        for (int col = 0; col < WIDTH; col++)
        {
            int c = fgetc(file);

            // Skip newline characters
            while (c == '\n' || c == '\r')
            {
                c = fgetc(file);
            }

            // Check if the file ends too early
            if (c == EOF)
            {
                fprintf(stderr, "Error: Map file is too small!\n");
                fclose(file);
                exit(1);
            }

            int index = get_index(row, col);

            // Convert the character into a Cell
            map[index] = char_to_cell((char)c);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[])
{
    // Check that the user provided a map filename
    if (argc != 2)
    {
        fprintf(stderr, "Please use: ./hackathon [Map_name]\n");
        return 1;
    }

    // Allocate memory for the map
    Cell *map = malloc(sizeof(Cell) * SIZE);

    if (map == NULL)
    {
        perror("Malloc failed");
        return 1;
    }

    // Load the map from the file
    load_map(argv[1], map);

    // Initialize ncurses
    initscr();
    noecho();
    nodelay(stdscr, true);
    keypad(stdscr, true);
    curs_set(false);

    // Initialize all colors
    initialize_colors();

    // Initialize the goat
    Goat goat;
    goat.hungry = false;
    goat.sleepy = false;
    goat.sleeping = false;
    goat.last_fed = time(NULL);
    goat.last_awake = time(NULL);
    goat.sleep_end = 0;

    bool running = true;
    int frame = 0;

    // Keep displaying the room until the player chooses to quit
    while (running)
    {
        // Update the goat's status
        update_goat(&goat);

        clear();

        // Print the room and goat
        print_board(map, &goat, frame);

        // Check keyboard input
        check_input(&running, &goat, map);

        frame++;
        sleep_ms(1000 / FRAME_RATE);
    }

    // Free allocated memory
    free(map);

    // Restore the terminal
    endwin();

    return 0;
}
