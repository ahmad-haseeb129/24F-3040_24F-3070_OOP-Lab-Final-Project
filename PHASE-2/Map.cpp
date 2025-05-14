#define _CRT_SECURE_NO_WARNINGS
#include "Stronghold.h"
#include <iostream>
#include <iomanip>
#include <string> 

Map::Map() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].kingdom[0] = '\0';
            grid[i][j].isOccupied = false;
        }
    }
    for (int i = 0; i < MAX_NAME_LEN; i++) {
        controlZones[i] = 0;
    }
}

void Map::moveKingdom(const char* kingdom, int newX, int newY) {
    if (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) {
        displayEvent("ERROR", "Invalid map coordinates!");
        return;
    }
    if (grid[newX][newY].isOccupied) {
        displayEvent("ERROR", "New location already occupied!");
        return;
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (strcmp(grid[i][j].kingdom, kingdom) == 0) {
                grid[i][j].kingdom[0] = '\0';
                grid[i][j].isOccupied = false;
                strncpy(grid[newX][newY].kingdom, kingdom, MAX_NAME_LEN - 1);
                grid[newX][newY].kingdom[MAX_NAME_LEN - 1] = '\0';
                grid[newX][newY].isOccupied = true;
                char message[100];
                std::string msg = std::string(kingdom) + " moved to (" + std::to_string(newX) + ", " + std::to_string(newY) + ")";
                strncpy(message, msg.c_str(), 99);
                message[99] = '\0';
                displayEvent("KINGDOM MOVED", message);
                return;
            }
        }
    }
    displayEvent("ERROR", "Kingdom not found on map!");
}

int Map::getDistance(const char* kingdom1, const char* kingdom2) {
    int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (strcmp(grid[i][j].kingdom, kingdom1) == 0) {
                x1 = i;
                y1 = j;
            }
            if (strcmp(grid[i][j].kingdom, kingdom2) == 0) {
                x2 = i;
                y2 = j;
            }
        }
    }
    if (x1 == -1 || x2 == -1) return 10; // Default large distance if not found
    return abs(x1 - x2) + abs(y1 - y2); // Manhattan distance
}

void Map::displayMap() 
{
    char buffer[500] = "Map (5x5):\n";
    for (int i = 0; i < GRID_SIZE; i++) 
    {
        for (int j = 0; j < GRID_SIZE; j++) 
        {
            if (grid[i][j].isOccupied) 
            {
                char temp[50];
                strcpy(temp, "[");
                snprintf(temp + strlen(temp), 4, "%-3.3s", grid[i][j].kingdom); // Show first 3 chars
                strcat(temp, "] ");
                strcat(buffer, temp);
            }
            else 
            {
                strcat(buffer, "[   ] ");
            }
        }
        strcat(buffer, "\n");
    }
    cout << "KINGDOM MAP\n\n";
    for (int i = 0; i < 500; i++)
    {
        cout << buffer[i];
    }
}

void Map::placeKingdom(const char* kingdom, int x, int y) 
{
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) 
    {
        displayEvent("ERROR", "Invalid map coordinates!");
        return;
    }
    if (grid[x][y].isOccupied) 
    {
        displayEvent("ERROR", "Location already occupied!");
        return;
    }
    strncpy(grid[x][y].kingdom, kingdom, MAX_NAME_LEN - 1);
    grid[x][y].kingdom[MAX_NAME_LEN - 1] = '\0';
    grid[x][y].isOccupied = true;
    controlZones[strlen(kingdom)]++;
    char message[100];
    strcpy(message, kingdom);
    strcat(message, " placed at (");
    std::string coord = std::to_string(x);
    strcat(message, coord.c_str());
    strcat(message, ", ");
    coord = std::to_string(y); 
    strcat(message, coord.c_str());
    strcat(message, ")");
    displayEvent("KINGDOM PLACED", message);
}

void Map::expandControl(const char* kingdom) 
{
    controlZones[strlen(kingdom)]++;
    char message[100];
    strcpy(message, kingdom);
    strcat(message, " expanded control zones to ");
    std::string zones = std::to_string(controlZones[strlen(kingdom)]); 
    strcat(message, zones.c_str());
    displayEvent("CONTROL EXPANDED", message);
}