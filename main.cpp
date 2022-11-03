#include <iostream>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <conio.h>
#include <time.h>

using namespace std;

const char SNAKE = '*';
const char SNAKE_HEAD = 'X';
const char FOOD = 'O';
const char SPECIAL = '@';

const int ROW = 10; // must be greater than 10
const int COL = 16; // must be greater than 15


double SPEED = 1.5; // define in seconds
const double SPEED_MULTIPLEX = 1.1; // multiple of speed

const int MAX_SNAKE_SIZE = ROW * COL;
const int POST_FRAME_SIZE = (COL + 5) * (ROW + 3) + 1;
const int SCORE_STRING_SIZE = 5;

enum Direction
{
    UP, DOWN, LEFT, RIGHT
};

struct Position
{
    int x = 0;
    int y = 0;
};

struct Snake
{
    Position positions[MAX_SNAKE_SIZE] = {COL / 2, ROW / 2};
    Direction direction = RIGHT;
    int tail = 0;
};

struct Game
{
    Snake snake;
    Position food;
    time_t timeRem;
    int score = 0;
    int foodEaten = 0;
    bool isHard = false;
} game;

static int frame[ROW][COL];

string timeToString(int seconds)
{
    char result[] = {'0', '0', ':', '0', '0'};
    int minute = seconds / 60;
    int second = seconds % 60;

    result[0] = '0' + minute / 10;
    result[1] = '0' + minute % 10;
    result[3] = '0' + second / 10;
    result[4] = '0' + second % 10;

    return result;
}

void exiting()
{
    cout << "Do you wants to exit?(" << timeToString(game.timeRem) << " Remaining) (Y/n)" << endl;
    char key;
    cin >> key;
    if (tolower(key) == 'y')
        exit(EXIT_SUCCESS);
    else
        cout << "Continued..." << endl;
}

void clearScreen()
{
    system("cls");
}

string scoreToString()
{
    char result[SCORE_STRING_SIZE];
    for (int i = 0; i < SCORE_STRING_SIZE; ++i)
        result[i] = '0' + (game.score / (int) pow(10, SCORE_STRING_SIZE - 1 - i)) % 10;
    return result;
}

void displayFrame()
{
    static char postFrame[POST_FRAME_SIZE];
    int position = 0;

    string timeStr = timeToString(game.timeRem);
    for (int i = 0; i < 5; ++i)
        postFrame[position++] = timeStr[i];

    for (int i = 0; i < COL - SCORE_STRING_SIZE - 5 + 4; ++i)
        postFrame[position++] = '.';

    string scoreStr = scoreToString();
    for (int i = 0; i < SCORE_STRING_SIZE; ++i)
        postFrame[position++] = scoreStr[i];

    postFrame[position++] = '\n';


    for (int i = 0; i < COL + 4; ++i)
        postFrame[position++] = '=';
    postFrame[position++] = '\n';

    char symbol[] = {' ', SNAKE, SNAKE_HEAD, FOOD, SPECIAL};
    for (int row = 0; row < ROW; ++row)
    {
        postFrame[position++] = '|';
        postFrame[position++] = '|';
        for (int col = 0; col < COL; ++col)
            postFrame[position++] = symbol[frame[row][col]];
        postFrame[position++] = '|';
        postFrame[position++] = '|';
        postFrame[position++] = '\n';
    }

    for (int i = 0; i < COL + 4; ++i)
        postFrame[position++] = '=';
    postFrame[position++] = '\n';

    clearScreen();
    puts(postFrame);
}

void note()
{
    string note = "NOTE!!!\n"
                  "This game is based on Nokia Snake Game\n"
                  "The purpose of the game is to make your mind faster\n"
                  "The object is simply eat the food provided\n"
                  "Use Numpad or ASWD to play game\n"
                  "The game have two mode\n"
                  "In easy mode your game ends on hitting wall\n"
                  "In hard mode the snake can cross walls\n";

    cout << note << endl;
    getch();
}

void setDifficulty()
{
    cout << "SELECT DIFFICULTY" << endl;
    cout << "1. Easy" << endl;
    cout << "2. Hard" << endl;

    int tries = 0;
    char option;
    while (true)
    {
        cout << "Enter Option: ";
        cin >> option;
        ++tries;

        if (option == '1' || option == '2')
        {
            game.isHard = option == '2';
            return;
        }
        else if (tries == 3)
        {
            cout << "Program is exiting due to multiple wrong tries...";
            getch();
            exit(EXIT_SUCCESS);
        }
        else
            cout << "Wrong Input..." << endl;
    }
}

int setTime()
{
    cout << "SELECT TIME" << endl;
    cout << "1. 30 seconds" << endl;
    cout << "2. 1 Minute" << endl;
    cout << "3. 2 Minute" << endl;
    cout << "4. 5 Minute" << endl;
    cout << "5. 10 Minute" << endl;

    int tries = 0;
    char option;
    while (true)
    {
        cout << "Enter Option: ";
        cin >> option;
        ++tries;

        if (option >= '1' && option <= '5')
            switch (option)
            {
            case '1':
                return 30;
            case '2':
                return 60;
            case '3':
                return 120;
            case '4':
                return 300;
            case '5':
                return 600;
            }
        else if (tries == 3)
        {
            cout << "Program is exiting due to multiple wrong tries...";
            getch();
            exit(EXIT_SUCCESS);
        }
        else
            cout << "Wrong Input..." << endl;
    }
}

bool contains(int x, int y)
{
    for (Position position: game.snake.positions)
        if (position.x == x && position.y == y)
            return true;
    return false;
}

void randFood()
{
    while (true)
    {
        int y = rand() % ROW;
        int x = rand() % COL;

        if (!contains(x, y))
        {
            game.food = {x, y};
            break;
        }
    }
}

bool move()
{
    auto headPos = game.snake.positions[0];
    static Position nextPos;

    switch (game.snake.direction)
    {
    case UP:
        nextPos = {headPos.x, headPos.y - 1};
        if (nextPos.y < 0)
        {
            if (game.isHard)
                nextPos.y = ROW - 1;
            else
                return false;
        }
        break;
    case DOWN:
        nextPos = {headPos.x, headPos.y + 1};
        if (nextPos.y >= ROW)
        {
            if (game.isHard)
                nextPos.y = 0;
            else
                return false;
        }
        break;
    case LEFT:
        nextPos = {headPos.x - 1, headPos.y};
        if (nextPos.x < 0)
        {
            if (game.isHard)
                nextPos.x = COL - 1;
            else
                return false;
        }
        break;
    case RIGHT:
        nextPos = {headPos.x + 1, headPos.y};
        if (nextPos.x >= COL)
        {
            if (game.isHard)
                nextPos.x = 0;
            else
                return false;
        }
        break;
    }

    frame[headPos.y][headPos.x] = 1;
    frame[nextPos.y][nextPos.x] = 2;

    bool isFoodEaten = nextPos.x == game.food.x && nextPos.y == game.food.y;
    if (isFoodEaten)
    {
        ++game.foodEaten;
        if (game.foodEaten % 5 == 0)
            game.score += 250;
        else
            game.score += 100;
        ++game.snake.tail;
        SPEED *= SPEED_MULTIPLEX;
    }
    else
    {
        auto tailPos = game.snake.positions[game.snake.tail];
        frame[tailPos.y][tailPos.x] = 0;
    }

    for (int i = 0; i < game.snake.tail - 1; ++i)
    {
        auto position = game.snake.positions[i];
        if (nextPos.x == position.x && nextPos.y == position.y)
            return false;
    }

    for (int i = game.snake.tail; i > 0; --i)
        game.snake.positions[i] = game.snake.positions[i - 1];
    game.snake.positions[0] = nextPos;

    if (isFoodEaten)
    {
        randFood();
        if ((game.foodEaten + 1) % 5 == 0)
            frame[game.food.y][game.food.x] = 4;
        else
            frame[game.food.y][game.food.x] = 3;
    }

    return true;
}

void start(int game_time)
{
    time_t start = time(nullptr);
    game.timeRem = game_time;
    randFood();

    frame[game.snake.positions[0].y][game.snake.positions[0].x] = 2;
    frame[game.food.y][game.food.x] = 3;

    displayFrame();

    char key = 0;
    time_t delay = 0;
    while (game.timeRem > 0)
    {
        game.timeRem = game_time + start - time(nullptr) + delay;

        usleep((int) (1000000 / SPEED));

        if (_kbhit())
        {
            key = tolower(_getch());

            switch (key)
            {
            case '4':
            case 'a':
                if (game.snake.direction != RIGHT)
                    game.snake.direction = LEFT;
                break;
            case '8':
            case'w':
                if (game.snake.direction != DOWN)
                    game.snake.direction = UP;
                break;
            case '6':
            case 'd':
                if (game.snake.direction != LEFT)
                    game.snake.direction = RIGHT;
                break;
            case '5' :
            case '2':
            case 's':
                if (game.snake.direction != UP)
                    game.snake.direction = DOWN;
                break;
            case 27:
                time_t temp = time(nullptr);
                exiting();
                delay += time(nullptr) - temp;
            }
        }

        if (!move()) break;
        displayFrame();
    }
    cout << "\nYou have scored " << game.score << endl;
    cout << "press any key to exit..." << endl;
    getch();
}

int main()
{
    srand(time(nullptr));

    clearScreen();
    note();

    clearScreen();
    setDifficulty();

    clearScreen();
    int game_time = setTime();

    clearScreen();
    cout << "press any key after 1 seconds to start..." << endl;
    sleep(1);
    getch();

    start(game_time);
    return 0;
}
