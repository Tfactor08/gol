#include <stdio.h>

#include <SDL2/SDL.h>

#define HEIGHT 480
#define WIDTH 640
#define ROWS 20
#define COLS 20
#define ROW_SIZE (double)HEIGHT / ROWS
#define COL_SIZE (double)WIDTH / COLS

#define WHITE 0xffffffff
#define GRAY  0xff181818
#define BLACK 0x0

SDL_Window *w;
SDL_Surface *s;

char cells[ROWS][COLS];

void draw_point(double x, double y, Uint32 color)
{
    SDL_Rect p = (SDL_Rect) {x, y, 1, 1};
    SDL_FillRect(s, &p, color);
}

void draw_grid()
{
    for (double y = 0; y <= HEIGHT; y += ROW_SIZE)
        for (int x = 0; x <= WIDTH; x++)
            draw_point(x, y, GRAY);
    for (double x = 0; x <= WIDTH; x += COL_SIZE)
        for (int y = 0; y <= HEIGHT; y++)
            draw_point(x, y, GRAY);
}

void fill_cell(int row, int col, char alive)
{
    double x = COL_SIZE * col + 1;
    double y = ROW_SIZE * row + 1;
    SDL_Rect cell_rect = (SDL_Rect){x, y, COL_SIZE-1, ROW_SIZE-1};
    if (alive)
        SDL_FillRect(s, &cell_rect, WHITE);
    else
        SDL_FillRect(s, &cell_rect, BLACK);
}

void init_cells()
{
    for(int i = 0; i < ROWS; i++)
        for(int j = 0; j < COLS; j++)
            cells[i][j] = 0;
}

void render_cells()
{
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            fill_cell(i, j, cells[i][j]);
}

// TODO
// there must a better way to manage the edge cases
int count_neighbours(int row, int col)
{
    int alive_cnt = 0;
    if (row != 0)
        alive_cnt += cells[row-1][col];
    if (row != ROWS-1)
        alive_cnt += cells[row+1][col];
    if (col != 0)
        alive_cnt += cells[row][col-1];
    if (col != COLS-1)
        alive_cnt += cells[row][col+1];
    if (!(row == 0 && col == 0))
        alive_cnt += cells[row-1][col-1];
    if (!(row == ROWS-1 && col == 0))
        alive_cnt += cells[row+1][col-1];
    if (!(row == 0 && col == COLS-1))
        alive_cnt += cells[row-1][col+1];
    if (!(row == ROWS-1 && col == COLS-1))
        alive_cnt += cells[row+1][col+1];

    return alive_cnt;
}

void eval_next_gen()
{
    char neighbours[ROWS][COLS];
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            neighbours[i][j] = count_neighbours(i, j);

    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            int neighbours_cnt = neighbours[i][j];
            if (cells[i][j]) {
                if (neighbours_cnt < 2)
                    cells[i][j] = 0;
                else if (neighbours_cnt > 3)
                    cells[i][j] = 0;
            }
            else {
                if (neighbours_cnt == 3)
                    cells[i][j] = 1;
            }
        }
}

// TODO
// for some reason, cells look streched in the window;
// refactor count_neighbours().
int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    w = SDL_CreateWindow("GOL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    s = SDL_GetWindowSurface(w);

    draw_grid();
    init_cells(cells);

    SDL_UpdateWindowSurface(w);

    SDL_Event e;
    while (1) {
        SDL_PollEvent(&e);
        if (e.type ==  SDL_QUIT) {
            SDL_Quit();
            break;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {       // create next generation
                eval_next_gen();
                render_cells();
                SDL_UpdateWindowSurface(w);
            }
            else if (e.button.button == SDL_BUTTON_RIGHT) { // add new alive cell
                int x = e.button.x;
                int y = e.button.y;
                int col = x / (int)(COL_SIZE);
                int row = y / (int)(ROW_SIZE);
                cells[row][col] = 1;
                fill_cell(row, col, 1);
                SDL_UpdateWindowSurface(w);
            }
        }

        SDL_Delay(10);
    }

    return 0;
}
