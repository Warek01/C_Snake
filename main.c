#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <stdint.h>
#include <time.h>

// For MSVC (VS) use _getchar and _kbhit
// instead of getchar and kbhit
// Also works for GNUC

#if defined(WIN32) || defined(_WIN32)
#define CLS "cls"
#include <windows.h>
#else
#define CLS "clear"
#include <unistd.h>
#endif

typedef int8_t Bool;

typedef struct
{
	int x;
	int y;
} Position;

typedef struct
{
	uint64_t start;
	uint64_t end;
	uint64_t diff;
} Timer;

typedef struct
{
	Position head;
	Position head_prev;
	Position tail_prev;
	Bool ate;
} Snake;

typedef struct timespec Time;

enum Game
{
	width = 11,
	height = 11,
	update_delay = 50,

	W = width,
	H = height,
	DELAY = update_delay
};

enum Key
{
	UP = 'w',
	DOWN = 's',
	LEFT = 'a',
	RIGHT = 'd',
	QUIT = 'q'
};

enum Square
{
	NONE,
	HEAD,
	FOOD,
	BODY
};

void nl(void);
int randint(int min, int max);
Position generate_food(int grid[W][H]);
void sleep_ms(unsigned int ms);
Bool is_movement_key(char key);
Bool check_movement(int grid[W][H], Position* head, char direction);

Snake snake;
Position food, body_nodes[W * H], byte_position;
Timer timer;
char key, movement_key;
int score = 0,
length = 0,
center_x, center_y,
grid[W][H];
Bool gameover;

int main(int argc, char** argv)
{
	srand(time(NULL));
	timer.start = time(NULL);

	for (int i = 0; i < W; i++)
		for (int j = 0; j < W; j++)
			grid[W][H] = NONE;

	// Center of grid
	center_x = floor((double)W / 2);
	center_y = floor((double)H / 2);

	// Start at center
	grid[center_x][center_y] = HEAD;

	snake.head.x = center_x;
	snake.head.y = center_y;
	snake.head_prev.x = 0;
	snake.head_prev.y = 0;
	snake.ate = 0;

	food = generate_food(grid);

	// Initial food position
	grid[food.x][food.y] = FOOD;

	system(CLS);

	// Top ##########
	for (int i = 0; i < W + 2; i++)
		printf("# ");

	nl();

	for (int i = 0; i < H; i++)
	{
		// Left bound
		putchar('#');

		for (int j = 0; j < W; j++)
		{
			if (grid[j][i] == NONE)
				printf("  ");
			else if (grid[j][i] == FOOD)
				printf(" @");
			else if (grid[j][i] == HEAD)
				printf(" 1");
			else if (grid[j][i] == BODY)
				printf(" 0");
		}

		// Right bound
		printf(" #");
		nl();
	}

	// Bottom ############
	for (int i = 0; i < W + 2; i++)
		printf("# ");

	nl();

	key = _getch();

	if (key == QUIT)
		exit(EXIT_SUCCESS);
	else if (is_movement_key(key))
		movement_key = key;

	while (!gameover)
	{
		sleep_ms(DELAY);
		system(CLS);

		if (_kbhit())
			key = _getch();

		if (key == 'q')
		{
			gameover = 1;
			break;
		}
		else if (
			is_movement_key(key) && 
			key != movement_key &&
			check_movement(grid, &snake.head, key)
		)
			movement_key = key;

		snake.head_prev.y = snake.head.y;
		snake.head_prev.x = snake.head.x;

		switch (movement_key)
		{
		case UP:
			grid[snake.head.x][snake.head.y] = 0;

			if (snake.head.y > 0)
			{
				grid[snake.head.x][snake.head.y - 1] = HEAD;
				snake.head.y--;
			}
			else
			{
				grid[snake.head.x][H - 1] = HEAD;
				snake.head.y = H - 1;
			}

			break;

		case DOWN:
			grid[snake.head.x][snake.head.y] = NONE;

			if (snake.head.y < H - 1)
			{
				grid[snake.head.x][snake.head.y + 1] = HEAD;
				snake.head.y++;
			}
			else
			{
				grid[snake.head.x][snake.head.y] = NONE;
				grid[snake.head.x][0] = HEAD;
				snake.head.y = 0;
			}
			break;

		case RIGHT:
			grid[snake.head.x][snake.head.y] = NONE;
			if (snake.head.x < W - 1)
			{
				grid[snake.head.x + 1][snake.head.y] = HEAD;
				snake.head.x++;
			}
			else
			{
				grid[0][snake.head.y] = HEAD;
				snake.head.x = 0;
			}
			break;
		case LEFT:
			grid[snake.head.x][snake.head.y] = NONE;
			if (snake.head.x > 0)
			{
				grid[snake.head.x - 1][snake.head.y] = HEAD;
				snake.head.x--;
			}
			else
			{
				grid[W - 1][snake.head.y] = HEAD;
				snake.head.x = W - 1;
			}
			break;
		}

		if (length > 0)
		{
			int x, y, tx, ty;

			x = body_nodes[0].x;
			y = body_nodes[0].y;
			body_nodes[0].x = snake.head_prev.x;
			body_nodes[0].y = snake.head_prev.y;
			grid[body_nodes[0].x][body_nodes[0].y] = BODY;

			

			if (length > 1)
				for (int i = 1; i < length; i++)
				{
					tx = body_nodes[i].x;
					ty = body_nodes[i].y;

					body_nodes[i].x = x;
					body_nodes[i].y = y;

					grid[body_nodes[i].x][body_nodes[i].y] = BODY;

					x = tx;
					y = ty;
				}
			grid[x][y] = NONE;

			snake.tail_prev.x = x;
			snake.tail_prev.y = y;
		}

		if (snake.ate)
		{
			Position pos = { snake.tail_prev.x, snake.tail_prev.y };
			body_nodes[length] = pos;
			length++;

			grid[snake.tail_prev.x][snake.tail_prev.y] = BODY;
			snake.ate = 0;
		}

		for (int i = 0; i < length; i++)
			if (snake.head.x == body_nodes[i].x && snake.head.y == body_nodes[i].y)
			{
				byte_position.x = snake.head.x;
				byte_position.y = snake.head.y;
				gameover = 1;
			}

		// Top ########
		for (int i = 0; i < W + 2; i++)
			printf("# ");

		nl();

		for (int i = 0; i < H; i++)
		{
			// Left bound
			putchar('#');

			for (int j = 0; j < W; j++)
			{
				if (grid[j][i] == NONE)
					printf("  ");
				else if (grid[j][i] == HEAD)
					printf(" 1");
				else if (grid[j][i] == FOOD)
					printf(" @");
				else if (grid[j][i] == BODY)
					printf(" 0");
			}

			// Right bound
			printf(" #");
			nl();
		}

		// bottom ##########
		for (int i = 0; i < W + 2; i++)
			printf("# ");

		nl();

		// On food pikup
		if (snake.head.x == food.x && snake.head.y == food.y)
		{
			snake.ate = 1;
			score++;

			food = generate_food(grid);
			grid[food.x][food.y] = FOOD;
		}

		switch (movement_key)
		{
		case UP:
			printf("UP");
			break;
		case DOWN:
			printf("DOWN");
			break;
		case LEFT:
			printf("LEFT");
			break;
		case RIGHT:
			printf("RIGHT");
			break;

		default:
			break;
		}

		printf("  |  SCORE: %d\n", score);
		printf("HEAD: (%d, %d)\n", snake.head.x, snake.head.y);
		printf("FOOD: (%d, %d)\n", food.x, food.y);
	}

	system("clear");

	// ENDSCREEN ------------------
	// Top ########
	for (int i = 0; i < W + 2; i++)
		printf("# ");

	nl();

	for (int i = 0; i < H; i++)
	{
		// Left bound
		putchar('#');

		for (int j = 0; j < W; j++)
		{
			if (j == byte_position.x && i == byte_position.y)
				printf(" X");
			else if (grid[j][i] == NONE)
				printf("  ");
			else if (grid[j][i] == HEAD)
				printf(" X");
			else if (grid[j][i] == FOOD)
				printf(" @");
			else if (grid[j][i] == BODY)
				printf(" 0");
		}

		// Right bound
		printf(" #");
		nl();
	}

	// bottom ##########
	for (int i = 0; i < W + 2; i++)
		printf("# ");

	nl();

	timer.end = time(NULL);
	timer.diff = timer.end - timer.start;

	printf("\n\tGAME OVER\n\n");
	printf("Score: %d  |  In-game time: %llu seconds\n", score, timer.diff);
	printf("---------------------\n");
	printf("Press any key to exit\n");

	_getch();

	exit(EXIT_SUCCESS);
}

// Newline
void nl(void)
{
	printf("\n");
}

int randint(int min, int max)
{
	return (rand() % (max - min + 1) + min);
}

// Generates random food coordinates which do not intersect given Head and Body of snake
Position generate_food(int grid[W][H])
{
	int x, y;
	do
	{
		x = randint(0, W - 1);
		y = randint(0, H - 1);
	} while (grid[x][y] != 0);

	Position food = { x, y };

	return food;
}

void sleep_ms(unsigned int ms)
{
#if defined(WIN32) || defined(_WIN32)
	Sleep(ms);
#else
	struct timespec delay = { 0, ms * 1000 * 1000 };
	nanosleep(&delay, NULL);
#endif
}

Bool is_movement_key(char key)
{
	return (int8_t)(key == UP || key == DOWN || key == LEFT || key == RIGHT);
}

Bool check_movement(int grid[W][H], Position *head, char direction)
{
	if (direction == UP && (head->y == 0 || grid[head->y - 1][head->x] != BODY))
		return 1;
	else if (direction == DOWN && (head->y == H - 1 || grid[head->y + 1][head->x] != BODY))
		return 1;
	else if (direction == LEFT && (head->x == 0 || grid[head->y][head->x - 1] != BODY))
		return 1;
	else if (direction == RIGHT && (head->x == W - 1 || grid[head->y][head->x + 1] != BODY))
		return 1;

	return 0;
}
