// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <getopt.h>
#include <deque>
#include <map>
#include <algorithm>
#include <unordered_set>
using namespace std;

enum class SearchMode 
{ 
  //search enum for command line parsing
  None = 0,
  Stack,
  Queue,
}; 

enum class OutputMode 
{ 
  //output enum for command line parsing
    None = 0,
    Map,
    List,
};

struct Options 
{ 
  //will convert these options from command line 
//into variables in main
  SearchMode searchMode = SearchMode::None;
  OutputMode outputMode = OutputMode::None;
}; 

struct Coordinate
{
int level;
int row;
int column;
Coordinate(int currentLevel, int currentRow,
int currentColumn);
Coordinate();
};

Coordinate::Coordinate()
:level(0),row(0),column(0)
{}

Coordinate::Coordinate(int currentLevel, int currentRow,
int currentColumn)
:level(currentLevel),row(currentRow),column(currentColumn)
{}

//need == operator for coordinate to use map
bool operator==(const Coordinate& lhs, const Coordinate& rhs)
{
  return (lhs.level == rhs.level 
        && lhs.row == rhs.row
        && lhs.column == rhs.column);
}
//need < operator for coordinte to use map
bool operator<(const Coordinate& lhs, const Coordinate& rhs)
{
  if(lhs.level != rhs. level) 
  {
    return lhs.level < rhs.level;
  }
  if(lhs.row != rhs.row) 
  {
    return lhs.row < rhs.row;
  }
    return lhs.column < rhs.column;
}

struct Tile
{
char character;
bool discovered;
char previousDirection;
Tile();
Tile(char tileChar, bool discovered, char prevDirection);
};

Tile::Tile(char tileChar, bool discovered, char prevDirection)
:character(tileChar),discovered(discovered),previousDirection(prevDirection){}

Tile::Tile()
:character('.'),discovered(false),previousDirection('\0'){}

bool operator!=(const Coordinate& lhs, const Coordinate& rhs)
{
  return 
  (lhs.level != rhs.level
  ||lhs.row != rhs.row
  ||lhs.column != rhs.column);
}
class Station
//station class for 3d storage and searching
{
private:
void printMap();
void printList();
vector<vector<vector<Tile>>> station;
deque<Coordinate> searchContainer;
vector<string> listPathPoints;
Coordinate startingPoint;
Coordinate hangarPoint;
int levels = 0;
int size = 0;
bool foundHangar = false;
bool stackSearch;
bool mapOutput;


public:
Station(char searchType, char outputType);
void fillStationFromMap();
void fillStationFromList();
void processElevator(const Coordinate &c);
void search();
void checkNorthCoordinate(const Coordinate &c);
void checkSouthCoordinate(const Coordinate &c);
void checkEastCoordinate(const Coordinate &c);
void checkWestCoordinate(const Coordinate &c);
void setStartingPoint(const Coordinate &c);
void print();
void printMapWithPath();
};
void Station::print()
{
  if(mapOutput)
  {
    printMap();
  }
  else
  {
    printList();
  }
}

Station::Station(char searchType, char outputType) 
//constructor, takes the types
//for searching and output from command line and converts them into internal 
//variables
:stackSearch(searchType == 's'),mapOutput(outputType == 'm')
{}
 

void Station::fillStationFromList()
//reads in the info about the station from a list mode input file
{
string levelVal;
getline(cin,levelVal);
levels = stoi(levelVal);
const auto vectorLevels = static_cast<uint32_t>(levels);
string sizeVal;
getline(cin,sizeVal);
size = stoi(sizeVal);
const auto vectorSize = static_cast<uint32_t>(size);
const Tile t;
station = vector<vector<vector<Tile>>>(vectorLevels,
vector<vector<Tile>>(vectorSize,
vector<Tile>(vectorSize,t)));
string currentRow;
while(cin >> currentRow)
{
  if(currentRow[0] == '/')
  {
    getline(cin,currentRow);
    continue;
  }
    vector<string> rowContents; 
    string currentContent;
    for(const char c: currentRow)
    {
        if(c != '(' && c != ')')
        {
        if(c!=',')
        {
            currentContent+=c;
        }
        else
        {
            rowContents.push_back(currentContent);
            currentContent = "";
        }
        }
    }
    rowContents.push_back(currentContent);
    const int currentLevel = stoi(rowContents[0]);
    const int currentRow = stoi(rowContents[1]);
    const int currentCol = stoi(rowContents[2]);
    const char currentChar = rowContents[3][0];
    if(currentChar != 'E' && currentChar != '#' 
    && currentChar != '.' && currentChar != 'H' 
    && currentChar != 'S')
    {
        cerr << "Invalid map character" << endl;
        exit(1);
    }
    if(currentLevel<0 || currentLevel>=levels)
    {
        cerr << "Invalid map level" << endl;
        exit(1);
    }
    if(currentRow<0 || currentRow>=size)
    {
        cerr << "Invalid map row" << endl;
        exit(1);
    }
    if(currentCol<0 || currentCol>=size)
    {
        cerr << "Invalid map column" << endl;
        exit(1);
    }
    if(currentChar == 'S')
    {
        const Coordinate coord(currentLevel,currentRow,currentCol);
        startingPoint = coord;
    }
    const auto vLevel = static_cast<u_int32_t>(currentLevel);
    const auto vRow = static_cast<u_int32_t>(currentRow);
    const auto vCol = static_cast<u_int32_t>(currentCol);
    station[vLevel][vRow][vCol].character = currentChar;
    char junk;
    cin >> junk;
}
}
void Station::fillStationFromMap()
//reads in the info about the station from a map mode input file
{
cin >> levels >> size;
const auto levelSize = static_cast<u_int32_t>(levels);
const auto sizeSize = static_cast<u_int32_t>(size);
const Tile t;
station = vector<vector<vector<Tile>>>(levelSize,
vector<vector<Tile>>(sizeSize,
vector<Tile>(sizeSize,t)));
string currentFileRow;
getline(cin,currentFileRow);
while(currentFileRow[0]=='/' || currentFileRow.empty()) {
    getline(cin,currentFileRow);
}
int currentLevelIndex = 0;
int currentRowIndex = 0;
while(!currentFileRow.empty())
{
if(currentFileRow[0]!='/')
{
int currentColIndex = 0;
for(const char c: currentFileRow)
{
    if(c != 'E' && c != '#' 
    && c != '.' && c != 'H' 
    && c != 'S')
    {
        cerr << "Invalid map character" << endl;
        exit(1);
    }
    if(currentColIndex == size)
    {
      currentColIndex--;
    }
    if(c == 'S')
    {
        const Coordinate coord(currentLevelIndex,currentRowIndex,currentColIndex);
        startingPoint = coord;
    }
    if(currentRowIndex == size)
    {
      currentRowIndex--;
    }
    const auto currLevel = static_cast<uint32_t>(currentLevelIndex);
    const auto currRow = static_cast<uint32_t>(currentRowIndex);
    const auto currColumn = static_cast<uint32_t>(currentColIndex);
    station[currLevel][currRow][currColumn].character = c;
    currentColIndex++;
}
currentRowIndex++;
currentColIndex = 0;
}
else
{
currentLevelIndex++;
currentRowIndex = 0;
}
getline(cin,currentFileRow);
}

}

void Station::processElevator(const Coordinate &c)
{
  for(int i=0; i<levels; i++)
        {
          const auto eIndex = static_cast<u_int32_t>(i);
          const auto eRow = static_cast<u_int32_t>(c.row);
          const auto eColumn = static_cast<u_int32_t>(c.column);
          const Coordinate elevator(i,c.row,c.column);
          if(!station[eIndex][eRow][eColumn].discovered && 
          station[eIndex][eRow][eColumn].character == 'E')
          {
          searchContainer.push_back(elevator);
          station[eIndex][eRow][eColumn].discovered = true;
          station[eIndex][eRow][eColumn].previousDirection = static_cast<char>(c.level+'0');
          }
        }
}

void Station::checkNorthCoordinate(const Coordinate &c)
{
    if(c.row >= 0)
    {
    const auto level = static_cast<u_int32_t>(c.level);
    const auto row = static_cast<u_int32_t>(c.row);
    const auto column= static_cast<u_int32_t>(c.column);
    if(station[level][row][column].character != '#'
    && !station[level][row][column].discovered)
    {
      searchContainer.push_back(c);
      station[level][row][column].discovered = true;
      station[level][row][column].previousDirection = 's';
      if (station[level][row][column].character == 'H')
      {
        hangarPoint.level = c.level;
        hangarPoint.row = c.row;
        hangarPoint.column = c.column;
        const auto hLevel = static_cast<uint32_t>(hangarPoint.level);
        const auto hRow = static_cast<uint32_t>(hangarPoint.row);
        const auto hColumn = static_cast<uint32_t>(hangarPoint.column);
        station[hLevel][hRow][hColumn].previousDirection = 's';
        foundHangar = true;
      }
    }
    }
}

void Station::checkSouthCoordinate(const Coordinate &c)
{
  if(c.row<size){
  const auto level = static_cast<u_int32_t>(c.level);
    const auto row = static_cast<u_int32_t>(c.row);
    const auto column= static_cast<u_int32_t>(c.column);
    if(station[level][row][column].character != '#'
    && !station[level][row][column].discovered)
    {
      searchContainer.push_back(c);
      station[level][row][column].discovered = true;
      station[level][row][column].previousDirection = 'n';
      if (station[level][row][column].character == 'H')
      {
        hangarPoint.level = c.level;
        hangarPoint.row = c.row;
        hangarPoint.column = c.column;
        const auto hLevel = static_cast<uint32_t>(hangarPoint.level);
        const auto hRow = static_cast<uint32_t>(hangarPoint.row);
        const auto hColumn = static_cast<uint32_t>(hangarPoint.column);
        station[hLevel][hRow][hColumn].previousDirection = 'n';
        foundHangar = true;
      }
    }
  }
  }


void Station::checkEastCoordinate(const Coordinate &c)
{
  if(c.column<size){
  const auto level = static_cast<u_int32_t>(c.level);
    const auto row = static_cast<u_int32_t>(c.row);
    const auto column= static_cast<u_int32_t>(c.column);
    if(station[level][row][column].character != '#'
    && !station[level][row][column].discovered)
    {
      searchContainer.push_back(c);
      station[level][row][column].discovered = true;
      station[level][row][column].previousDirection = 'w';
      if (station[level][row][column].character == 'H')
      {
        hangarPoint.level = c.level;
        hangarPoint.row = c.row;
        hangarPoint.column = c.column;
        const auto hLevel = static_cast<uint32_t>(hangarPoint.level);
        const auto hRow = static_cast<uint32_t>(hangarPoint.row);
        const auto hColumn = static_cast<uint32_t>(hangarPoint.column);
        station[hLevel][hRow][hColumn].previousDirection = 'w';
        foundHangar = true;
      }
    }
    }
}


void Station::checkWestCoordinate(const Coordinate &c)
{
    if(c.column >= 0)
    {
    const auto level = static_cast<u_int32_t>(c.level);
    const auto row = static_cast<u_int32_t>(c.row);
    const auto column= static_cast<u_int32_t>(c.column);
    if(station[level][row][column].character != '#'
    && !station[level][row][column].discovered)
    {
      searchContainer.push_back(c);
      station[level][row][column].discovered = true;
      station[level][row][column].previousDirection = 'e';
      if (station[level][row][column].character == 'H')
      {
        hangarPoint.level = c.level;
        hangarPoint.row = c.row;
        hangarPoint.column = c.column;
        const auto hLevel = static_cast<uint32_t>(hangarPoint.level);
        const auto hRow = static_cast<uint32_t>(hangarPoint.row);
        const auto hColumn = static_cast<uint32_t>(hangarPoint.column);
        station[hLevel][hRow][hColumn].previousDirection = 'e';
        foundHangar = true;
      }
    }
    }
}

void Station::search()
{
  searchContainer.push_back(startingPoint);
  const auto startLevel = static_cast<uint32_t>(startingPoint.level);
  const auto startRow = static_cast<uint32_t>(startingPoint.row);
  const auto startColumn = static_cast<uint32_t>(startingPoint.column);
  station[startLevel][startRow][startColumn].discovered = true;
  station[startLevel][startRow][startColumn].previousDirection = '\0';
  while(!searchContainer.empty())
  {
    Coordinate next;
    if(stackSearch) 
    {
      next = searchContainer.back();
      searchContainer.pop_back();
    }
    else
    {
      next = searchContainer.front();
      searchContainer.pop_front();
    }
    //check north location
    const Coordinate north(next.level,next.row-1,next.column);
    checkNorthCoordinate(north);
    if(foundHangar)
    {
      break;
    }
    const Coordinate east(next.level,next.row,next.column+1);
    checkEastCoordinate(east);
    if(foundHangar)
    {
      break;
    }
    const Coordinate south(next.level,next.row+1,next.column);
    checkSouthCoordinate(south);
    if(foundHangar)
    {
      break;
    }
    const Coordinate west(next.level,next.row,next.column-1);
    checkWestCoordinate(west);
    if(foundHangar)
    {
      break;
    }
    const auto currLevel = static_cast<uint32_t>(next.level);
    const auto currRow = static_cast<uint32_t>(next.row);
    const auto currColumn = static_cast<uint32_t>(next.column);
    if(station[currLevel][currRow][currColumn].character == 'E')
    {
      processElevator(next);
    }
}
}
void Station::setStartingPoint(const Coordinate &c)
{
if(c.row > startingPoint.row)
{
  const auto row = static_cast<u_int32_t>(startingPoint.row);
  const auto column = static_cast<u_int32_t>(startingPoint.column);
  const auto level = static_cast<u_int32_t>(startingPoint.level);
  station[level][row][column].character = 's';
}
else if(c.row < startingPoint.row)
{
  const auto row = static_cast<u_int32_t>(startingPoint.row);
  const auto column = static_cast<u_int32_t>(startingPoint.column);
  const auto level = static_cast<u_int32_t>(startingPoint.level);
  station[level][row][column].character = 'n';
}
else if(c.column > startingPoint.column)
{
  const auto row = static_cast<u_int32_t>(startingPoint.row);
  const auto column = static_cast<u_int32_t>(startingPoint.column);
  const auto level = static_cast<u_int32_t>(startingPoint.level);
  station[level][row][column].character = 'e';
}
else
{
  const auto row = static_cast<u_int32_t>(startingPoint.row);
  const auto column = static_cast<u_int32_t>(startingPoint.column);
  const auto level = static_cast<u_int32_t>(startingPoint.level);
  station[level][row][column].character = 'w';
}
}

void Station::printList()
{
cout<<"//path taken\n";
Coordinate currentPoint = hangarPoint;
while(currentPoint != startingPoint)
{
  if(!foundHangar)
  {
    break;
  }
  const auto cLevel = static_cast<uint32_t>(currentPoint.level);
  const auto cRow = static_cast<uint32_t>(currentPoint.row);
  const auto cColumn= static_cast<uint32_t>(currentPoint.column);
  const char currentPrevDirection = station[cLevel][cRow][cColumn].previousDirection;
  if(currentPrevDirection == 'n')
  {
    currentPoint.row--;
    string coordToAdd;
    coordToAdd +="("+to_string(currentPoint.level)+","+
    to_string(currentPoint.row)+","+
    to_string(currentPoint.column)+",s)";
    listPathPoints.push_back(coordToAdd);
  }
  else if(currentPrevDirection == 's')
  {
    currentPoint.row++;
    string coordToAdd;
    coordToAdd +="("+to_string(currentPoint.level)+","+
    to_string(currentPoint.row)+","+
    to_string(currentPoint.column)+",n)";
    listPathPoints.push_back(coordToAdd);
  }
  else if(currentPrevDirection == 'e')
  {
    currentPoint.column++;
    string coordToAdd;
    coordToAdd +="("+to_string(currentPoint.level)+","+
    to_string(currentPoint.row)+","+
    to_string(currentPoint.column)+",w)";
    listPathPoints.push_back(coordToAdd);
  }
  else if(currentPrevDirection == 'w')
  {
    currentPoint.column--;
    string coordToAdd;
    coordToAdd +="("+to_string(currentPoint.level)+","+
    to_string(currentPoint.row)+","+
    to_string(currentPoint.column)+",e)";
    listPathPoints.push_back(coordToAdd);
  }
  else //elevator so currentPrevDirection is num betwween 0-9
  {
    const int currentLevel = currentPoint.level;
    currentPoint.level = currentPrevDirection -'0';
    string coordToAdd;
    coordToAdd +="("+to_string(currentPoint.level)+","+
    to_string(currentPoint.row)+","+
    to_string(currentPoint.column)+","+
    to_string(currentLevel) +")";
    listPathPoints.push_back(coordToAdd);
  }
  
}
reverse(listPathPoints.begin(),listPathPoints.end());
for(const auto &i: listPathPoints)
{
  cout<<i<<"\n";
}
}

void Station::printMap()
{
cout<<"Start in level "<<startingPoint.level<<", row "<<startingPoint.row
<<", column "<<startingPoint.column<<"\n";  
Coordinate currentPoint = hangarPoint;
Coordinate lastPoint;
while(currentPoint != startingPoint)
{
  if(!foundHangar)
  {
    break;
  }
  const auto cLevel = static_cast<uint32_t>(currentPoint.level);
  const auto cRow = static_cast<uint32_t>(currentPoint.row);
  const auto cColumn= static_cast<uint32_t>(currentPoint.column);
  const char currentPrevDirection = station[cLevel][cRow][cColumn].previousDirection;
  if(currentPrevDirection == 'n')
  {
    lastPoint = currentPoint;
    const auto newRow = static_cast<uint32_t>(cRow-1);
    station[cLevel][newRow][cColumn].character = 's';
    currentPoint.row--;
  }
  else if(currentPrevDirection == 's')
  {
    lastPoint = currentPoint;
    const auto newRow = static_cast<uint32_t>(cRow+1);
    station[cLevel][newRow][cColumn].character = 'n';
    currentPoint.row++;
  }
  else if(currentPrevDirection == 'e')
  {
    lastPoint = currentPoint;
    const auto newColumn = static_cast<uint32_t>(cColumn+1);
    station[cLevel][cRow][newColumn].character = 'w';
    currentPoint.column++;
  }
  else if(currentPrevDirection == 'w')
  {
    lastPoint = currentPoint;
    const auto newColumn = static_cast<uint32_t>(cColumn-1);
    station[cLevel][cRow][newColumn].character = 'e';
    currentPoint.column--;
  }
  else //elevator so currentPrevDirection is num betwween 0-9
  {
    lastPoint = currentPoint;
    const auto newLevel = static_cast<uint32_t>(currentPrevDirection-'0');
    station[newLevel][cRow][cColumn].character = static_cast<char>(currentPoint.level+'0');
    currentPoint.level = currentPrevDirection -'0';
  }
}
if(foundHangar)
{
setStartingPoint(lastPoint);
}
printMapWithPath();
}

void Station::printMapWithPath()
{
  for(int i=0; i<levels; i++)
{
  cout<<"//level "+to_string(i)+"\n";
  for(int j=0; j<size; j++)
  {
    for(int k=0; k<size; k++)
    {
      const auto cLevel = static_cast<uint32_t>(i);
      const auto cRow = static_cast<uint32_t>(j);
      const auto cColumn = static_cast<uint32_t>(k);
      cout<<station[cLevel][cRow][cColumn].character; 
    }
      cout<<"\n";
    }
  }
}


void printHelp(char *argv[]) 
{
  cout << "Usage: " << argv[0] << " [-m resize|reserve|nosize] | -h\n";
  cout << "This program is to help you learn command-line processing,\n";
  cout << "reading data into a vector, the difference between resize and\n";
  cout << "reserve and how to properly read until end-of-file." << endl;
} 

void setModes(int argc, char * argv[], Options &options) 
{
  // uses getopt_long() to parse command line for modes as well as error checking
  opterr = false;
  int choice;
  int index = 0;
  option long_options[] = {
    {"stack", no_argument,        nullptr, 's'},
    {"queue", no_argument,        nullptr, 'q'},
    {"output", required_argument, nullptr, 'o'},
    {"help",   no_argument,      nullptr, 'h' },
    { nullptr, 0, nullptr, '\0' }
  };


  while ((choice = getopt_long(argc, argv, "sqo:h", long_options, &index)) != -1) {
    switch (choice) {
      case 'h':
        printHelp(argv);
        exit(0);

      case 'o': {
        if(!optarg) 
        {
          options.outputMode = OutputMode::Map;
        }
        else if (*optarg == 'L') 
        {
          options.outputMode = OutputMode::List;
        }
        else
        {
          options.outputMode = OutputMode::Map;
        }
        break;
      }

     case 's': {
        if(options.searchMode!=SearchMode::None)
        {
            cerr << "Multiple routing modes specified" << endl;
            cerr << "Multiple stack/queue commands found" << endl;
            exit(1);
        }
        options.searchMode = SearchMode::Stack;
        break;
     }
     case 'q': {
        if(options.searchMode!=SearchMode::None)
        {
            cerr << "Multiple routing modes specified" << endl;
            cerr << "Multiple stack/queue commands found" << endl;
            exit(1);
        }
        options.searchMode = SearchMode::Queue;
        break;
     }
    }  // switch ..choice
  }  // while

  if (options.searchMode == SearchMode::None) {
    cerr << "Error: No routing mode specified" << endl;
    cerr << "Must include either a stack or queue command" << endl;
    exit(1);
  }  // if ..mode
} 

int main(int argc, char * argv[]) 
{
    ios_base::sync_with_stdio(false);
    Options options;
    setModes(argc,argv,options);
    char searchType;
    char outputType;
    if(options.searchMode == SearchMode::Queue) {
      searchType = 'q';
    }
    else 
    {
      searchType = 's';
    }

    if(options.outputMode == OutputMode::List) 
    {
      outputType = 'l';
    }
    else
    {
      outputType = 'm';
    }

    Station station(searchType,outputType);
    string inputType;
    getline(cin,inputType);
    if(inputType == "M") {
      station.fillStationFromMap();
    }
    else 
    {
      station.fillStationFromList();
    }
    station.search();
    station.print();
}
