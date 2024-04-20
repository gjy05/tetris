#include "tetromino.h"
#include "console/console.h"

#include <iostream>

Tetromino Tetromino::I = Tetromino("I", 4, "XXXXOOOOXXXXXXXX");
Tetromino Tetromino::O = Tetromino("O", 2, "OOOO");
Tetromino Tetromino::T = Tetromino("T", 3, "XOXOOOXXX");
Tetromino Tetromino::S = Tetromino("S", 3, "XOOOOXXXX");
Tetromino Tetromino::Z = Tetromino("Z", 3, "OOXXOOXXX");
Tetromino Tetromino::J = Tetromino("J", 3, "OXXOOOXXX");
Tetromino Tetromino::L = Tetromino("L", 3, "XXOOOOXXX");

Tetromino::Tetromino(std::string name, int size, std::string shape)
{
    name_ = name;
    size_ = size;

    // shape_[x][y] 와 같은 방식으로 init 함
    for (int i = 0; i < size_ * size_; i++)
    { // (size가 3일 경우) i % size_ == 0, 1, 2, 0, 1, 2...
        shape_[i % size_][i / size_] = (shape[i] == 'O');
    } // i / size_ == 0, 0, 0, 1, 1, 1, 2, 2, 2

    if (name == "I")
    {
        original_ = &Tetromino::I;
    }
    else if (name == "O")
    {
        original_ = &Tetromino::O;
    }
    else if (name == "T")
    {
        original_ = &Tetromino::T;
    }
    else if (name == "S")
    {
        original_ = &Tetromino::S;
    }
    else if (name == "Z")
    {
        original_ = &Tetromino::Z;
    }
    else if (name == "J")
    {
        original_ = &Tetromino::J;
    }
    else if (name == "L")
    {
        original_ = &Tetromino::L;
    }
    else
    {
        std::cout << "name error" << std::endl;
    }
}

Tetromino Tetromino::rotatedCCW()
{
    std::string shape_rotated_CCW;
    for (int x = size() - 1; 0 <= x; x--)
    {
        for (int y = 0; y < size(); y++)
        {
            if (check(x, y) == true)
            {
                shape_rotated_CCW.push_back('O');
            }
            else
            {
                shape_rotated_CCW.push_back('X');
            }
        }
    }
    return Tetromino(name_, size_, shape_rotated_CCW);
}
Tetromino Tetromino::rotatedCW() // 세 번 반시계방향으로 돌리기 == 한 번 시계방향으로 돌리기
{
    return rotatedCCW().rotatedCCW().rotatedCCW();
}

void Tetromino::drawAt(std::string s, int x, int y)
{
    for (int tet_x = 0; tet_x < size(); tet_x++)
    {
        for (int tet_y = 0; tet_y < size(); tet_y++)
        {
            if (check(tet_x, tet_y) == true)
            {
                console::draw(x + tet_x, y + tet_y, s);
            }
        }
    }
}