/*
    pathfinder.cpp

    author: Kaih White
*/

#define print(x) cout << x << endl;

#include <iostream>
#include "pathfinder.h"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <deque>
#include <iomanip>

using namespace std;

pathfinder::pathfinder() { ; }

// public run() method - invokes private methods to do everything;
// returns the total elevation change cost of the best optimal path.
int pathfinder::run(string data_file_name, string save_file_name, bool use_recursion) {
    // suggested implementation - modify as desired!
    _use_recursion = use_recursion;
    if (!_read_data(data_file_name)) {
        cout << "Error reading map data from \"" << data_file_name << "\"!" << endl;
        return -1;
    }

    _draw_map();

    int best_cost = _draw_paths();

    _save_image(save_file_name);

    return best_cost;
}

/*******************/
/* PRIVATE METHODS */
/*******************/

// _read_data - read and store width, height, and elevation data from the
// provided data file; return false if something goes wrong
bool pathfinder::_read_data(string data_file_name) {
    ifstream coords;

    coords.open(data_file_name);
    string line;
    string placeholder;

    for (int i = 0; i < 2; i++) {
        getline(coords, line);
        istringstream sin(line);
        while (!sin.eof()) {
            if (i == 0) {
                sin >> placeholder;
                if (placeholder.find_first_not_of("0123456789") == std::string::npos) {
                    _width = stoi(placeholder);
                }
            } else if (i == 1) {
                sin >> placeholder;
                if (placeholder.find_first_not_of("0123456789") == std::string::npos) {
                    _height = stoi(placeholder);
                }
            }
        }
    }

    vector<int> vplace;
    int p = 0;
    while (!coords.eof()) {
        if (p > 1) {
            vplace.clear();
            for (int a = 0; a < _width; a++) {
                coords >> placeholder;
                vplace.push_back(stoi(placeholder));
            }
            _elevations.push_back(vplace);
        }
        p++;
    }

    coords.close();

    return true;
}

// _draw_map - draw the elevation data as grayscale values on our Picture
// object.
void pathfinder::_draw_map() {
    double max = INT_MIN;
    double min = INT_MAX;
    for (int ro = 0; ro < _height; ro++) {
        for (int co = 0; co < _width; co++) {
            if (_elevations[ro][co] > max) {
                max = _elevations[ro][co];
            }
            if (_elevations[ro][co] < min) {
                min = _elevations[ro][co];
            }
        }
    }

    scale = (255.0) / (max - min);

    for (int row = 0; row < _height; row++) {
        for (int column = 0; column < _width; column++) {
            int r;
            r = ((_elevations[row][column]) - min) * scale;

            _image.set(row, column, r, r, r);
        }
    }
}

int pathfinder::_optCostToEast(int row, int col, int y, int x, vector<vector<int>> elevations) {
    int cost = 0;
    int cost1 = 0;
    int cost2 = 0;
    if (x == col - 1) {
        // base case
        return 0;
    }
    if (x < col - 1) {
        cost = abs(elevations[y][x] - elevations[y][x + 1]) + _optCostToEast(row, col, y, x + 1, elevations);
        if (y < row - 1) {
            cost1 = abs(elevations[y][x] - elevations[y + 1][x + 1]) +
                    _optCostToEast(row, col, y + 1, x + 1, elevations);
        }
        if (y > 0) {
            cost2 = abs(elevations[y][x] - elevations[y - 1][x + 1]) +
                    _optCostToEast(row, col, y - 1, x + 1, elevations);
        }
    }
    if (cost1 != 0) {
        if (cost1 < cost) {
            cost = cost1;
        }
    }
    if (cost2 != 0) {
        if (cost2 < cost) {
            cost = cost2;
        }
    }
    return cost;
}

int pathfinder::dynamo(int row, int col, vector<vector<int>> elevations) {
    deque<deque<int>> cmap(row);
    deque<deque<int>> dmap(row);

    for (int j = col; j > 0; j--) {
        for (int i = 0; i < row; i++) {
            int c1 = 0;
            int c2 = 0;
            int c3 = 0;
            int d1 = 0;
            int d2 = 0;
            int d3 = 0;
            if (j == col) {
                cmap[i].push_back(0);
                dmap[i].push_back(0);
            } else {
                c1 = abs(elevations[i][j - 1] - elevations[i][j]) + cmap[i][0];
                d1 = 0;
                if (i < row - 1) {
                    c2 = abs(elevations[i][j - 1] - elevations[i + 1][j]) + cmap[i + 1][0];
                    d2 = -1;
                }
                if (i > 0) {
                    c3 = abs(elevations[i][j - 1] - elevations[i - 1][j]) + cmap[i - 1][1];
                    d3 = 1;
                }
                if (c2 != 0) {
                    if (c2 < c1) {
                        c1 = c2;
                        d1 = d2;
                    }
                }
                if (c3 != 0) {
                    if (c3 < c1) {
                        d1 = d3;
                        c1 = c3;
                    }
                }
                cmap[i].push_front(c1);
                dmap[i].push_front(d1);
            }
        }
    }

    for (int rownum = 0; rownum < _height; rownum++) {
        int rpoint = rownum;
        for (int k = 0; k < _width; k++) {
            _image.set(rpoint, k, 0, 0, 255);
            switch (dmap[rpoint][k]) {
                case 0:
                    break;
                case 1:
                    rpoint--;
                    break;
                case -1:
                    rpoint++;
                    break;
            }
        }
    }
    int x = 0;
    int y = 0;
    int start = 0;
    int minstart = INT_MAX;

    while (y < _height - 1) {
        if (cmap[y][x] < minstart) {
            minstart = cmap[y][x];
            start = y;
        }
        y++;
    }
    while (x < _width) {
        _image.set(start, x, 255, 255, 0);
        switch (dmap[start][x]) {
            case 0:
                x++;
                break;
            case 1:
                x++;
                start--;
                break;
            case -1:
                x++;
                start++;
                break;
        }
    }
    //return best optimal path
    return minstart;
}

// _draw_paths() - find and draw optimal paths from west to east using either recursion or dynamic programming
// return the best optimal path cost
int pathfinder::_draw_paths() {
    vector<int> allcosts;
    int optimal = INT_MAX;

    if (_use_recursion) {
        print("Hi")
        for (int p = 0; p < _height; p++) {
            allcosts.push_back(_optCostToEast(_height, _width, p, 0, _elevations));
        }
        for (int c = 0; c < _height; c++) {
            if (allcosts[c] < optimal) {
                optimal = allcosts[c];
            }
        }
    } else {
        optimal = dynamo(_height, _width, _elevations);
    }
    return optimal;
}

void pathfinder::_save_image(string save_file_name) {
    _image.save(save_file_name);
}

