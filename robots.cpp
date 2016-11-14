// robots.cpp

// Portions you are to complete are marked with a TODO: comment.
// We've provided some incorrect return statements (so indicated) just
// to allow this skeleton program to compile and run, albeit incorrectly.
// The first thing you probably want to do is implement the trivial
// functions (marked TRIVIAL).  Then get Arena::display going.  That gives
// you more flexibility in the order you tackle the rest of the functionality.
// As you finish implementing each TODO: item, remove its TODO: comment.

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;             // max number of rows in the arena
const int MAXCOLS = 30;             // max number of columns in the arena
const int MAXROBOTS = 100;          // max number of robots allowed

const int UP    = 0;
const int DOWN  = 1;
const int LEFT  = 2;
const int RIGHT = 3;

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir);
int randInt(int min, int max);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
// type name, since it's mentioned in the Robot declaration.

class Robot
{
public:
    // Constructor
    Robot(Arena* ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    
    // Mutators
    void move();
    bool getAttacked(int dir);
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int health;
};

class Player
{
public:
    // Constructor
    Player(Arena *ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;
    
    // Mutators
    void   stand();
    void   moveOrAttack(int dir);
    void   setDead();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_age;
    bool   m_dead;
};

class Arena
{
public:
    // Constructor/destructor
    Arena(int nRows, int nCols);
    ~Arena();
    
    // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     robotCount() const;
    int     nRobotsAt(int r, int c) const;
    bool    determineNewPosition(int& r, int& c, int dir) const;
    void    display() const;
    
    // Mutators
    bool   addRobot(int r, int c);
    bool   addPlayer(int r, int c);
    bool   attackRobotAt(int r, int c, int dir);
    bool   moveRobots();
    
private:
    int     m_rows;
    int     m_cols;
    Player* m_player;
    Robot*  m_robots[MAXROBOTS];
    int     m_nRobots;
};

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nRobots);
    ~Game();
    
    // Mutators
    void play();
    
private:
    Arena* m_arena;
};

///////////////////////////////////////////////////////////////////////////
//  Robot implementation
///////////////////////////////////////////////////////////////////////////

/**
    Robot Constructor
    @param the arena it exists on
    @param the row it appears on
    @param the column it appears on
 */
Robot::Robot(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** A robot must be in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "***** Robot created with invalid coordinates (" << r << ","
        << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    health = 2;
}

/**
    Returns the row that the robot is on
 */
int Robot::row() const
{
    return m_row;
}

/**
    Returns the column that the robot is on
 */
int Robot::col() const
{
    return m_col;
}

/**
    Moves the robot in a random direction
 */
void Robot::move()
{
    // Attempt to move in a random direction; if we can't move, don't move
    int dir = randInt(0, 3);  // dir is now UP, DOWN, LEFT, or RIGHT
    
    m_arena->determineNewPosition(m_row, m_col, dir);
}

/**
    Modifies the status of a robot once it is attacked
    @return true if it dies
    @return false if it lives
 */
bool Robot::getAttacked(int dir)
{
    // Robot has been attacked once before
    if (health == 1)
        return true;
    
    health--;
    
    // Checks to see whether the robot was hit against a wall boundary
    bool isNotHitAgainstWall = m_arena->determineNewPosition(m_row, m_col, dir);
    
    // Robot was hit against the wall and dies
    if(!isNotHitAgainstWall)
        return true;
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////
//  Player implementations
///////////////////////////////////////////////////////////////////////////

/**
    Player constructor
    @param the arena it exists on
    @param the row it appears on
    @param the column it appears on
 */
Player::Player(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** The player must be in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
        << "," << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_age = 0;
    m_dead = false;
}

/**
    Returns the row that the player is on
    @return row position
 */
int Player::row() const
{
    return m_row;
}

/**
    Returns the column that the player is on
    @return column position
 */
int Player::col() const
{
    return m_col;
}

/**
    Returns the player's age
 */
int Player::age() const
{
    return m_age;
}

/**
    Increments the player's age
 */
void Player::stand()
{
    m_age++;
}

void Player::moveOrAttack(int dir)
{
    m_age++;
    int numRobots = 0;
    switch (dir)
    {
        case UP:
            if (m_row - 1 >= 1)
            {
                int positionUp = m_row - 1;
                
                // Finds the number or robots in the cell above
                numRobots = m_arena->nRobotsAt(positionUp, m_col);
                
                // If there is at least one robot, attack the cell above
                if (numRobots != 0)
                {
                    m_arena->attackRobotAt(positionUp, m_col, dir);
                    return;
                }
            }
            break;
        case DOWN:
            if (m_row + 1 <= m_arena->rows())
            {
                int positionDown = m_row + 1;
                numRobots = m_arena->nRobotsAt(positionDown, m_col);
                if (numRobots != 0)
                {
                    m_arena->attackRobotAt(positionDown, m_col, dir);
                    return;
                }
            }
            break;
        case LEFT:
            if (m_col - 1 >= 1)
            {
                int positionLeft = m_col - 1;
                numRobots = m_arena->nRobotsAt(m_row, positionLeft);
                if (numRobots != 0)
                {
                    m_arena->attackRobotAt(m_row, positionLeft, dir);
                    return;
                }
            }
            break;
        case RIGHT:
            if (m_col + 1 <= m_arena->cols())
            {
                int positionRight = m_col + 1;
                numRobots = m_arena->nRobotsAt(m_row, positionRight);
                if (numRobots != 0)
                {
                    m_arena->attackRobotAt(m_row, positionRight, dir);
                    return;
                }
            }
            break;
    }
    
    // Runs only if space in specified direction does not have any robots
    m_arena->determineNewPosition(m_row, m_col, dir);
}

/**
    Returns whether the player is alive
 */
bool Player::isDead() const
{
    return m_dead;
}

/**
    Sets the player's 'daed' data member to true
 */
void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementations
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
    if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
    {
        cout << "***** Arena created with invalid size " << nRows << " by "
        << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nRobots = 0;
}

Arena::~Arena()
{
    // Releases memory that stored the player
    delete m_player;
    
    // Releases memory that stored the robots
    for (int i = 0; i < m_nRobots; i++)
    {
        delete m_robots[i];
    }
}

/**
    Returns the number of rows in the arena
 */
int Arena::rows() const
{
    return m_rows;
}

/**
    Returns the number of columns in the arena
 */
int Arena::cols() const
{
    return m_cols;
}

/**
    Returns a pointer to the player in the arena
 */
Player* Arena::player() const
{
    return m_player;
}

/**
    Returns the number of robots
    @return how many robots are still in the arena
 */
int Arena::robotCount() const
{
    return m_nRobots;
}

/**
    Finds the number of robots that are currently occupying a certain cell
    @param the row position in question
    @param the column position in question
    @returns the number of robots at this cell
 */
int Arena::nRobotsAt(int r, int c) const
{
    int count = 0;
    
    for (int i = 0; i < m_nRobots; i++)
    {
        if (m_robots[i] != nullptr && m_robots[i]->row() == r && m_robots[i]->col() == c)
            count++;
    }
    return count;
}

/**
    Determines whether a move is within the bounds of the grid
    @returns whether the move was successful or not
 */
bool Arena::determineNewPosition(int& r, int& c, int dir) const
{
    switch (dir)
    {
        case UP:
            // Checks whether the cell above is a valid position
            if (r - 1 < 1)
                break;
            else
            {
                r--;
                return true;
            }
            
        case DOWN:
            // Checks whether the cell below is a valid position
            if (r + 1 > m_rows)
                break;
            else
            {
                r++;
                return true;
            }
            
        case LEFT:
            // Checks whether the cell to the left is a valid position
            if (c - 1 < 1)
                break;
            else
            {
                c--;
                return true;
            }
            
        case RIGHT:
            // Checks whether the cell to the right is a valid position
            if (c + 1 > m_cols)
                break;
            else
            {
                c++;
                return true;
            }
        default:
            return false;
    }
    return false;
}

void Arena::display() const
{
    // Position (row,col) in the arena coordinate system is represented in
    // the array element grid[row-1][col-1]
    char grid[MAXROWS][MAXCOLS];
    int r, c;
    
    // Fill the grid with dots
    for (r = 0; r < rows(); r++)
        for (c = 0; c < cols(); c++)
            grid[r][c] = '.';
    
    // Indicate each robot's position
    for (int row = 0; row < rows(); row++)
    {
        for (int col = 0; col < cols(); col++)
        {
            // Grid [0,0] is arena's [1,1]
            int numRobots = nRobotsAt(row + 1, col + 1);
            
            if (numRobots == 1)
            {
                grid[row][col] = 'R';
            }
            
            if (2 <= numRobots && numRobots <= 9)
            {
                grid[row][col] = '0' + numRobots;
            }
            
            if (numRobots > 9)
            {
                grid[row][col] = '9';
            }
        }
    }
        
    
    // Indicate player's position
    if (m_player != nullptr)
    {
        // Set the char to '@', unless there's also a robot there,
        // in which case set it to '*'.
        char& gridChar = grid[m_player->row()-1][m_player->col()-1];
        if (gridChar == '.')
            gridChar = '@';
        else
            gridChar = '*';
    }
    
    // Draw the grid
    clearScreen();
    for (r = 0; r < rows(); r++)
    {
        for (c = 0; c < cols(); c++)
            cout << grid[r][c];
        cout << endl;
    }
    cout << endl;
    
    // Write message, robot, and player info
    cout << endl;
    cout << "There are " << robotCount() << " robots remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player." << endl;
    else
    {
        if (m_player->age() > 0)
            cout << "The player has lasted " << m_player->age() << " steps." << endl;
        if (m_player->isDead())
            cout << "The player is dead." << endl;
    }
}

bool Arena::addRobot(int r, int c)
{
    // If MAXROBOTS have already been added, return false.  Otherwise,
    // dynamically allocate a new robot at coordinates (r,c).  Save the
    // pointer to the newly allocated robot and return true.
    
    if (m_nRobots >= MAXROBOTS)
        return false;
    
    // Dynamically allocate a new robot and add it to the arena
    // Adds the robot pointer to the arena's array
    m_robots[m_nRobots] = new Robot(this, r, c);
    m_nRobots++;
    return true;
}

/**
    Adds a Player object to the class
    @param the player's row position
    @param the player's column position
 */
bool Arena::addPlayer(int r, int c)
{
    // Don't add a player if one already exists
    if (m_player != nullptr)
        return false;
    
    // Dynamically allocate a new Player and add it to the arena
    m_player = new Player(this, r, c);
    return true;
}

/**
    Attacks a robot at a given position
    @param the robot's row position
    @param the robot's column position
    @param the direction the robot is attacked at
    @return whether an attack was successful
 */
bool Arena::attackRobotAt(int r, int c, int dir)
{
    for (int i = 0; i < m_nRobots; i++)
    {
        if (m_robots[i] != nullptr && m_robots[i]->row() == r && m_robots[i]->col() == c)
        {
            // Attacks a robot
            bool robotHasDied = m_robots[i]->getAttacked(dir);

            // If the robot is dead, delete it and shift the array to remove the nullptr
            if (robotHasDied)
            {
                delete m_robots[i];
                for (int j = i; j < m_nRobots - 1; j++)
                {
                    m_robots[j] = m_robots[j+1];
                }
                m_nRobots--;
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

/**
    Moves all of the robots in the arena
    @returns whether the player is still alive
 */
bool Arena::moveRobots()
{
    for (int k = 0; k < m_nRobots; k++)
    {
        m_robots[k]->move();
        
        if (m_robots[k] != nullptr && m_player != nullptr && m_robots[k]->col() == m_player->col() && m_robots[k]->row() == m_player->row())
        {
            m_player->setDead();
        }
    }
    
    // return true if the player is still alive, false otherwise
    return ! m_player->isDead();
}

///////////////////////////////////////////////////////////////////////////
//  Game implementations
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRobots)
{
    if (nRobots < 0)
    {
        cout << "***** Cannot create Game with negative number of robots!" << endl;
        exit(1);
    }
    if (nRobots > MAXROBOTS)
    {
        cout << "***** Trying to create Game with " << nRobots
        << " robots; only " << MAXROBOTS << " are allowed!" << endl;
        exit(1);
    }
    if (rows == 1  &&  cols == 1  &&  nRobots > 0)
    {
        cout << "***** Cannot create Game with nowhere to place the robots!" << endl;
        exit(1);
    }
    
    // Create arena
    m_arena = new Arena(rows, cols);
    
    // Add player
    int rPlayer = randInt(1, rows);
    int cPlayer = randInt(1, cols);
    m_arena->addPlayer(rPlayer, cPlayer);
    
    // Populate with robots
    while (nRobots > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        // Don't put a robot where the player is
        if (r == rPlayer  &&  c == cPlayer)
            continue;
        m_arena->addRobot(r, c);
        nRobots--;
    }
}

Game::~Game()
{
    delete m_arena;
}

void Game::play()
{
    Player* p = m_arena->player();
    if (p == nullptr)
    {
        m_arena->display();
        return;
    }
    do
    {
        m_arena->display();
        cout << endl;
        cout << "Move (u/d/l/r//q): ";
        string action;
        getline(cin,action);
        if (action.size() == 0)  // player stands
            p->stand();
        else
        {
            switch (action[0])
            {
                default:   // if bad move, nobody moves
                    cout << '\a' << endl;  // beep
                    continue;
                case 'q':
                    return;
                case 'u':
                case 'd':
                case 'l':
                case 'r':
                    p->moveOrAttack(decodeDirection(action[0]));
                    break;
            }
        }
        m_arena->moveRobots();
    } while ( ! m_arena->player()->isDead()  &&  m_arena->robotCount() > 0);
    m_arena->display();
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementations
///////////////////////////////////////////////////////////////////////////

int decodeDirection(char dir)
{
    switch (dir)
    {
        case 'u':  return UP;
        case 'd':  return DOWN;
        case 'l':  return LEFT;
        case 'r':  return RIGHT;
    }
    return -1;  // bad argument passed in!
}

// Return a random int from min to max, inclusive
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

///////////////////////////////////////////////////////////////////////////
//  main()
///////////////////////////////////////////////////////////////////////////

int main()
{
    // Create a game
    // Use this instead to create a mini-game: Game g(3, 4, 2);
    Game g(7, 8, 25);
    
    // Play the game
    g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                               &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
    static const char* term = getenv("TERM");
    if (term == nullptr  ||  strcmp(term, "dumb") == 0)
        cout << endl;   
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
    }
}

#endif