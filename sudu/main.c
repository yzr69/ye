#include <stdio.h>
#include <stdlib.h>

// Sudoku board data (0 means empty)
int board[9][9] = {
    {0, 0, 0, 0, 0, 0, 0, 9, 0},
    {1, 9, 0, 4, 7, 0, 6, 0, 8},
    {0, 5, 2, 8, 1, 9, 4, 0, 7},
    {2, 0, 0, 0, 4, 8, 0, 0, 0},
    {0, 0, 9, 0, 0, 0, 5, 0, 0},
    {0, 0, 0, 7, 5, 0, 0, 0, 9},
    {9, 0, 7, 3, 6, 4, 1, 8, 0},
    {5, 0, 6, 0, 8, 1, 0, 7, 4},
    {0, 8, 0, 0, 0, 0, 0, 0, 0}
};

// ========== Game global variables ==========
int player_board[9][9];
int answer_board[9][9];
int original_board[9][9];
int error_count = 0;

// Print formatted Sudoku board
void print_board(int board[][9]) {
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) printf(" | ");
            else printf(" ");
            if (board[i][j] == 0) printf("_");
            else printf("%d", board[i][j]);
        }
        printf(" |\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
}

// Check if a number is valid at (row, col)
int isValid(int number, int puzzle[][9], int row, int col) {
    int rowStart = (row / 3) * 3;
    int colStart = (col / 3) * 3;
    for (int i = 0; i < 9; i++) {
        if (puzzle[row][i] == number) return 0;
        if (puzzle[i][col] == number) return 0;
        if (puzzle[rowStart + (i / 3)][colStart + (i % 3)] == number) return 0;
    }
    return 1;
}

// Recursive Sudoku solver
int solve(int puzzle[][9], int pos) {
    if (pos == 81) return 1;
    int row = pos / 9, col = pos % 9;
    if (puzzle[row][col] != 0) return solve(puzzle, pos + 1);
    for (int num = 1; num <= 9; num++) {
        if (isValid(num, puzzle, row, col)) {
            puzzle[row][col] = num;
            if (solve(puzzle, pos + 1)) return 1;
            puzzle[row][col] = 0;
        }
    }
    return 0;
}

// Initialize game state
void init_game() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            original_board[i][j] = board[i][j];
            player_board[i][j] = board[i][j];
            answer_board[i][j] = board[i][j];
        }
    solve(answer_board, 0);
    error_count = 0;
    printf("Game initialized!\n");
}

// Handle user input
int handle_input() {
    int row, col, num;
    printf("Enter row col number (1-9), or 0 0 0 to quit: ");
    scanf("%d %d %d", &row, &col, &num);
    if (row == 0 && col == 0 && num == 0) return -1;
    if (row < 1 || row > 9 || col < 1 || col > 9 || num < 1 || num > 9) {
        printf("Out of range! Please enter numbers 1-9.\n");
        return 0;
    }
    row--; col--;
    if (original_board[row][col] != 0) {
        printf("This cell is fixed and cannot be changed!\n");
        return 0;
    }
    if (player_board[row][col] != 0) {
        printf("This cell is already filled!\n");
        return 0;
    }
    if (answer_board[row][col] == num) {
        player_board[row][col] = num;
        printf("Correct!\n");
        return 1;
    } else {
        error_count++;
        printf("Wrong! Error count: %d\n", error_count);
        return 1;
    }
}

// Check if the game is complete
int is_complete() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (player_board[i][j] == 0) return 0;
    return 1;
}

// Save board to text file
void save_to_text_file(int board[][9], const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Cannot open file %s for writing\n", filename);
        return;
    }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) fprintf(fp, ".");
            else fprintf(fp, "%d", board[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("Saved to %s\n", filename);
}

// Read board from text file
int read_from_text_file(int board[][9], const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s for reading\n", filename);
        return 0;
    }
    char line[20];
    int row = 0;
    while (row < 9 && fgets(line, sizeof(line), fp) != NULL) {
        for (int col = 0; col < 9; col++) {
            if (line[col] == '.') board[row][col] = 0;
            else if (line[col] >= '1' && line[col] <= '9') board[row][col] = line[col] - '0';
        }
        row++;
    }
    fclose(fp);
    if (row < 9) {
        printf("Warning: file format incorrect or incomplete\n");
        return 0;
    }
    printf("Loaded board from %s\n", filename);
    return 1;
}

// Game main loop
void play_game() {
    printf("=== Sudoku Game ===\n");
    printf("Rule: Enter row col number to fill a cell\n");
    printf("Game ends after 5 mistakes\n\n");
    init_game();
    printf("Initial board:\n");
    print_board(player_board);
    while (error_count < 5) {
        int result = handle_input();
        if (result == -1) {
            printf("Game ended by user.\n");
            break;
        }
        if (result == 1) {
            printf("\nCurrent board:\n");
            print_board(player_board);
            if (is_complete()) {
                printf("🎉 Congratulations! You solved the Sudoku!\n");
                break;
            }
        }
    }
    if (error_count >= 5) {
        printf("💥 Too many mistakes, game over!\n");
        printf("Correct answer:\n");
        print_board(answer_board);
    }
}

int main() {
    play_game();
    return 0;
}