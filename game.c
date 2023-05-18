#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define DEAD_CELL 0
#define ALIVE_CELL 1
#define SCREEN_ALIVE 0
#define SCREEN_DEAD ' '
#define UP_SPEED 'u'
#define DOWN_SPEED 'd'

/*
create_matrix() - Создаёт матрицу с размерами SCREEN_WIDTH * SCREEN_HEIGHT
clear_matrix() - Принимает матрицу, которую нужно очистить
fill_matrix() - Заполняет матрицу клетками из файла
ouput_matrix() - Вывод матрицы
count_neib() - Возвращает количество соседей с учетом бесконечного поля
update_cell() - Определяет живет ли клетка или умирает и возращает её состояние
(жизнь/смерть)
update_frame() - Заносит в основную матрицу все изменения из
второй матрицы
*/

int **create_matrix();
void clear_matrix(int **matrix);
void fill_matrix(int **matrix);
void output_matrix(int **matrix);
int count_neib(int **matrix, int cell_i, int cell_j);
int update_cell(int **matrix, int count, int cell_i, int cell_j);

void update_frame(int **playing_field, int **change_frame);
int change_speed(int current_speed, char button);

int main() {
    int **playing_field = create_matrix(), **change_frame = create_matrix();
    int error = 0;
    WINDOW *win;
    win = initscr();

    fill_matrix(playing_field);
    if (!freopen("/dev/tty", "r", stdin)) {
        printf("n/a");
        error = 1;
    }
    int res, cell_status, current_speed = 50000;

    nodelay(win, 1);
    noecho();
    while (error == 0) {
        char var1 = getch();
        if (var1 == 'q') {
            break;
        }
        current_speed = change_speed(current_speed, var1);

        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                res = count_neib(playing_field, i, j);
                cell_status = update_cell(playing_field, res, i, j);

                if (cell_status == 1) {
                    change_frame[i][j] = ALIVE_CELL;
                } else {
                    change_frame[i][j] = DEAD_CELL;
                }
            }
        }

        update_frame(playing_field, change_frame);
        output_matrix(playing_field);

        refresh();
        usleep(current_speed);
    }
    clear_matrix(playing_field);
    clear_matrix(change_frame);
    endwin();

    return 0;
}

int **create_matrix() {
    int **matrix = (int **)malloc(SCREEN_HEIGHT * sizeof(int *));
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        matrix[i] = (int *)malloc(SCREEN_WIDTH * sizeof(int));
    }
    return matrix;
}

void clear_matrix(int **matrix) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void fill_matrix(int **matrix) {
    int cell = 0;
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            scanf("%d", &cell);
            if (cell == 1) {
                matrix[i][j] = ALIVE_CELL;
            } else {
                matrix[i][j] = DEAD_CELL;
            }
        }
    }
    printw("\n");
}

int update_cell(int **matrix, int count, int cell_i, int cell_j) {
    /*
            matrix - текущий кадр
            count - кол-во живых соседей около этой клетки
            cell_i / cell_j - координаты клетки

            res - какое будет состояние клетки на след. кадре
            (1 = ALIVE) / (0 = DEAD)
    */
    int res;
    if (matrix[cell_i][cell_j] == ALIVE_CELL) {
        if (count < 2 || count > 3) {
            res = 0;
        } else {
            res = 1;
        }
    } else {
        if (count == 3) {
            res = 1;
        } else {
            res = 0;
        }
    }
    return res;
}

void update_frame(int **playing_field, int **change_frame) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            playing_field[i][j] = change_frame[i][j];
        }
    }
}

int count_neib(int **matrix, int cell_i, int cell_j) {
    /*
        **matrix - текущий кадр
        cell_i / cell_j - координаты клетки, вокруг которой мы считаем живых
       соседей neib_i / neib_j - координаты соседей, эта переменная меняется в
       цикле count - счетчик, который считает живых соседей вокруг клетки

    */
    int neib_i, neib_j, count = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }

            neib_i = cell_i + i;
            neib_j = cell_j + j;

            neib_i = (neib_i + SCREEN_HEIGHT) % SCREEN_HEIGHT;
            neib_j = (neib_j + SCREEN_WIDTH) % SCREEN_WIDTH;

            if (matrix[neib_i][neib_j] == ALIVE_CELL) {
                count++;
            }
        }
    }
    return count;
}

int change_speed(int current_speed, char button) {
    if (button == UP_SPEED) {
        current_speed /= 2;
    }
    if (button == DOWN_SPEED) {
        current_speed *= 2;
    }
    return current_speed;
}

void output_matrix(int **matrix) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        move(i, 0);
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            if (j < SCREEN_WIDTH - 1) {
                if (matrix[i][j] == ALIVE_CELL) {
                    printw("%d", SCREEN_ALIVE);
                } else {
                    printw("%c", SCREEN_DEAD);
                }
            }
        }
        // printf("\n");
    }
    printw("\n");
}

