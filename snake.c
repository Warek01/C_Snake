#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <math.h>
#include <stdint.h>

#if defined(WIN32) || defined(_WIN32)
  #define CLS "cls"
#elif defined(__unix__) || defined(linux)
  #define CLS "clear"
#endif

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
  int ate;
} Snake;

typedef struct timespec Time;

#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

#define W 19
#define H 9
#define UPDATE_DELAY 200
#define MS UPDATE_DELAY * 1000 * 1000
#define HEAD 1
#define FOOD 2
#define BODY 3
#define NONE 0

Time delay = {0, MS};
Snake snake;
Position food, center, body_nodes[W * H], byte_position;
Timer timer;
char key;
int score = 0,
    gameover = 0,
    length = 0,
    grid[W][H];

void nl(void);
int randint(int, int);
Position generate_food(int[W][H]);

int main(void)
{
  srand(time(NULL));
  timer.start = time(NULL);

  for (int i = 0; i < W; i++)
    for (int j = 0; j < W; j++)
      grid[W][H] = NONE;

  // Center of grid
  center.x = floor((double)W / 2);
  center.y = floor((double)H / 2);

  // Start at center
  grid[center.x][center.y] = HEAD;

  snake.head.x = center.x;
  snake.head.y = center.y;
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

  printf("\nSCORE: %d\n", score);
  printf("HEAD-X: %d  HEAD-Y: %d\n", snake.head.x, snake.head.y);
  printf("FOOD-X: %d  FOOD-Y: %d\n", food.x, food.y);

  nl();

  key = getch();

  if (key == 'q')
    exit(EXIT_SUCCESS);

  while (!gameover)
  {
    nanosleep(&delay, NULL);
    system(CLS);

    if (kbhit())
      key = getch();

    if (key == 'q')
    {
      gameover = 1;
      break;
    }

    snake.head_prev.y = snake.head.y;
    snake.head_prev.x = snake.head.x;

    switch (key)
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
      grid[snake.head.x][snake.head.y] = 0;

      if (snake.head.y < H - 1)
      {
        grid[snake.head.x][snake.head.y + 1] = 1;
        snake.head.y++;
      }
      else
      {
        grid[snake.head.x][snake.head.y] = 0;
        grid[snake.head.x][0] = 1;
        snake.head.y = 0;
      }
      break;

    case RIGHT:
      grid[snake.head.x][snake.head.y] = 0;
      if (snake.head.x < W - 1)
      {
        grid[snake.head.x + 1][snake.head.y] = 1;
        snake.head.x++;
      }
      else
      {
        grid[0][snake.head.y] = 1;
        snake.head.x = 0;
      }
      break;
    case LEFT:
      grid[snake.head.x][snake.head.y] = 0;
      if (snake.head.x > 0)
      {
        grid[snake.head.x - 1][snake.head.y] = 1;
        snake.head.x--;
      }
      else
      {
        grid[W - 1][snake.head.y] = 1;
        snake.head.x = W - 1;
      }
      break;
    }

    if (snake.ate)
    {
      // 0 body Nodes (create it on grid)
      Position pos = {snake.head_prev.x, snake.head_prev.y};
      body_nodes[length] = pos;
      length++;

      grid[snake.head_prev.x][snake.head_prev.y] = BODY;
      snake.ate = 0;
    }
    else if (length > 0)
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

    switch (key)
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
    printf("HEAD-X: %d  HEAD-Y: %d\n", snake.head.x, snake.head.y);
    printf("FOOD-X: %d  FOOD-Y: %d\n", food.x, food.y);

    nl();
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
  printf("Press any key to exit\n\n");

  getch();

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

  Position food = {x, y};

  return food;
}
