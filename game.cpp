#include "game.h"
#include "console/console.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

Game::Game() : start_time_(std::chrono::steady_clock::now()),
               seconds_taken_(0),
               lines_left_(LINES),
               state_of_game_(0),
               cur_tet_(make_rand_tet()),
               next_tet_(make_rand_tet()),
               hold_tet_(make_rand_tet()),
               cur_tet_x_(5 - (cur_tet_.size() / 2)), // size() 가 2, 3, 4 일때 각각 4, 4, 3
               cur_tet_y_(0),
               shadow_y_(BOARD_HEIGHT - 2),
               fixed_cur_(false),
               have_held_(false),
               can_hold_(true)
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            board_[x][y] = false;
        }
    } // 처음에는 모두 비어 있으므로 배열 안의 값을 모두 false 로 초기화
}
void Game::update()
{
    handle_input();

    int seconds_taken = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start_time_).count();

    if (seconds_taken_ < seconds_taken || fixed_cur_)
    { // 1초가 지났거나 하드드롭되어 현재 테트로미노가 고정되었거나
        seconds_taken_ = seconds_taken;

        if (should_cur_to_board())
        {
            cur_to_board();
            delete_lines();

            if (lines_left_ <= 0)
            {
                state_of_game_ = 1;
            }
            else if (can_new_tet(next_tet_))
            {
                new_tet(next_tet_);
                next_tet_ = make_rand_tet();
                can_hold_ = true;
            }
            else
            {
                state_of_game_ = -1;
            }
        }
        else // cur_tet 을 한칸 내린다. 다 내린 경우는 위에서 검사했으므로 고려할 필요가 없다.
        {
            cur_tet_y_++;
        }
    }
}
void Game::draw()
{
    draw_border();
    draw_lines_left();
    draw_time(2, BOARD_HEIGHT + 3);
    draw_fixed_blocks();
    draw_next_tet_and_hold_tet();
    draw_cur_tet_and_shadow();

    switch (state_of_game_)
    {
    case -1:
        draw_lost();
        break;
    case 1:
        draw_win();
        break;
    default:
        break;
    }
}
bool Game::shouldExit()
{
    switch (state_of_game_)
    {
    case -2:
        return true;
        break;
    case -1:
        return true;
        break;
    case 0:
        return false;
        break;
    case 1:
        return true;
        break;
    }
    return false;
}

Tetromino Game::make_rand_tet()
{
    Tetromino *rand_tet;
    int random = rand() % 7;
    switch (random)
    {
    case 0:
        rand_tet = &Tetromino::I;
        break;
    case 1:
        rand_tet = &Tetromino::O;
        break;
    case 2:
        rand_tet = &Tetromino::T;
        break;
    case 3:
        rand_tet = &Tetromino::S;
        break;
    case 4:
        rand_tet = &Tetromino::Z;
        break;
    case 5:
        rand_tet = &Tetromino::J;
        break;
    case 6:
        rand_tet = &Tetromino::L;
        break;

    default:
        std::cout << "make_rand_tet() error" << std::endl;
        break;
    }
    return *rand_tet;
}

void Game::handle_input()
{
    if (console::key(console::K_ESC))
    {
        state_of_game_ = -2;
        return;
    }
    // 하드 드롭 한 이후에는 fixed_cur_ 이 false 가 될 때까지 강제 종료를 제외한 어떠한 행동도 불가능하게 해야 한다
    if (!fixed_cur_) // 하드 드롭하지 않은 경우
    {
        if (console::key(console::K_UP))
        { // 하드 드롭
            cur_tet_y_ = shadow_y_;
            fixed_cur_ = true;
        }
        if (console::key(console::K_DOWN))
        { // 소프트 드롭
            if (can_soft())
            {
                cur_tet_y_++;
            }
        }
        if (console::key(console::K_LEFT))
        { // 왼쪽 이동
            if (can_left())
            {
                cur_tet_x_--;
                calculate_shadow_y();
            }
        }
        if (console::key(console::K_RIGHT))
        { // 오른쪽 이동
            if (can_right())
            {
                cur_tet_x_++;
                calculate_shadow_y();
            }
        }
        if (console::key(console::K_Z))
        { // 반시계방향 회전
            if (can_rotate(cur_tet_.rotatedCCW()))
            {
                cur_tet_ = cur_tet_.rotatedCCW();
                calculate_shadow_y();
            }
        }
        if (console::key(console::K_X))
        { // 시계방향 회전
            if (can_rotate(cur_tet_.rotatedCW()))
            {
                cur_tet_ = cur_tet_.rotatedCW();
                calculate_shadow_y();
            }
        }
        if (console::key(console::K_SPACE))
        { // hold
            if (can_hold_)
            {
                hold_tet();
            }
        }
    }
}

void Game::calculate_shadow_y()
{
    int min_shadow_y = BOARD_HEIGHT - 2;
    for (int x = 0; x < cur_tet_.size(); x++)
    {
        for (int y = 0; y < cur_tet_.size(); y++)
        {
            if (cur_tet_.check(x, y))
            {
                for (int shadow_y = cur_tet_y_; shadow_y + y + 1 <= BOARD_HEIGHT; shadow_y++)
                {
                    if (shadow_y + y + 1 == BOARD_HEIGHT)
                    {
                        if (shadow_y < min_shadow_y)
                        {
                            min_shadow_y = shadow_y;
                        }
                    }
                    else if (board_[cur_tet_x_ + x][shadow_y + y + 1] && shadow_y < min_shadow_y)
                    {
                        min_shadow_y = shadow_y;
                    }
                }
            }
        }
    }
    shadow_y_ = min_shadow_y;
}

bool Game::can_left()
{
    for (int x = 0; x < cur_tet_.size(); x++)
    {
        for (int y = 0; y < cur_tet_.size(); y++)
        {
            // 테트로미노 좌측상단 기준 x, y 좌표에 블록이 있다면
            if (cur_tet_.check(x, y))
            { // board_ 배열에서 인덱싱이 불가능하면 튕기므로 벽과 겹치거나 넘어가는지 미리 검사해줌
                if (cur_tet_x_ + x - 1 < 0)
                { // 값이 0 미만인지만 보면 된다
                    return false;
                }
                // board_ 배열에 좌표를 적절한 좌표를 집어넣어 블록이 겹치는지 확인
                if (board_[cur_tet_x_ + x - 1][cur_tet_y_ + y])
                {
                    return false;
                }
            }
        }
    }
    return true;
}
bool Game::can_right()
{
    for (int x = 0; x < cur_tet_.size(); x++)
    {
        for (int y = 0; y < cur_tet_.size(); y++)
        {
            // 테트로미노 좌측상단 기준 x, y 좌표에 블록이 있다면
            if (cur_tet_.check(x, y))
            { // board_ 배열에서 인덱싱이 불가능하면 튕기므로 벽과 겹치거나 넘어가는지 미리 검사해줌
                if (BOARD_WIDTH <= cur_tet_x_ + x + 1)
                { // 값이 BOARD_WIDTH 이상인지만 보면 된다
                    return false;
                }
                // board_ 배열에 좌표를 적절한 좌표를 집어넣어 블록이 겹치는지 확인
                if (board_[cur_tet_x_ + x + 1][cur_tet_y_ + y])
                {
                    return false;
                }
            }
        }
    }
    return true;
}
bool Game::can_soft()
{
    for (int x = 0; x < cur_tet_.size(); x++)
    {
        for (int y = 0; y < cur_tet_.size(); y++)
        {
            // 테트로미노 좌측상단 기준 x, y 좌표에 블록이 있다면
            if (cur_tet_.check(x, y))
            { // board_ 배열에서 인덱싱이 불가능하면 튕기므로 벽과 겹치거나 넘어가는지 미리 검사해줌
                if (BOARD_HEIGHT <= cur_tet_y_ + y + 1)
                { // 값이 BOARD_HEIGHT 이상인지만 보면 된다
                    return false;
                }
                // board_ 배열에 좌표를 적절한 좌표를 집어넣어 블록이 겹치는지 확인
                if (board_[cur_tet_x_ + x][cur_tet_y_ + y + 1])
                {
                    return false;
                }
            }
        }
    }
    return true;
}
bool Game::can_rotate(Tetromino rotated_tet)
{
    for (int x = 0; x < rotated_tet.size(); x++)
    {
        for (int y = 0; y < rotated_tet.size(); y++)
        {
            // 테트로미노 좌측상단 기준 x, y 좌표에 블록이 있다면
            if (rotated_tet.check(x, y))
            { // board_ 배열에서 인덱싱이 불가능하면 튕기므로 벽과 겹치거나 넘어가는지 미리 검사해줌
                if (cur_tet_x_ + x < 0 || BOARD_WIDTH <= cur_tet_x_ + x ||
                    cur_tet_y_ + y < 0 || BOARD_HEIGHT <= cur_tet_y_ + y)
                { // 회전은 전 방향을 다 봐야 한다.
                    return false;
                }
                // board_ 배열에 좌표를 적절한 좌표를 집어넣어 겹치는지 확인
                if (board_[cur_tet_x_ + x][cur_tet_y_ + y])
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool Game::can_new_tet(Tetromino new_tet)
{
    // board_ 에서의 cur_tet_x_, cur_tet_y_ 의 위치
    int x_if_new = 5 - (new_tet.size() / 2);
    int y_if_new = 0;

    for (int x = 0; x < new_tet.size(); x++)
    {
        for (int y = 0; y < new_tet.size(); y++)
        {
            if (new_tet.check(x, y) && board_[x_if_new + x][y_if_new + y])
            {
                return false;
            }
        }
    }
    return true;
}
void Game::new_tet(Tetromino new_tet)
{
    cur_tet_ = new_tet;
    can_hold_ = true;
    fixed_cur_ = false;

    cur_tet_x_ = 5 - (cur_tet_.size() / 2);
    cur_tet_y_ = 0;
    
    calculate_shadow_y();
}
void Game::hold_tet()
{
    if (have_held_)
    {
        if (can_new_tet(hold_tet_))
        {
            Tetromino tmp_tet = *cur_tet_.original();
            new_tet(hold_tet_);
            hold_tet_ = tmp_tet;
        }
        else
        {
            state_of_game_ = -1;
        }
    }
    else
    {
        if (can_new_tet(next_tet_))
        {
            hold_tet_ = *cur_tet_.original();
            new_tet(next_tet_);
            next_tet_ = make_rand_tet();
            have_held_ = true;
        }
        else
        {
            state_of_game_ = -1;
        }
    }
    can_hold_ = false;
}

bool Game::should_cur_to_board()
{
    return cur_tet_y_ == shadow_y_;
}
void Game::cur_to_board()
{
    for (int x = 0; x < cur_tet_.size(); x++)
    {
        for (int y = 0; y < cur_tet_.size(); y++)
        {
            if (cur_tet_.check(x, y))
            {
                board_[cur_tet_x_ + x][cur_tet_y_ + y] = true;
            }
        }
    }
}
void Game::delete_lines()
{
    for (int y = BOARD_HEIGHT - 1; 0 <= y; y--)
    { // 아래 부터 검사해서 지우고 바로 위를 한 칸씩 내린다.
        bool should_delete = true;
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (board_[x][y] == false)
            {
                should_delete = false;
                break;
            }
        }

        if (should_delete)
        {
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                board_[x][y] = false;
                for (int down_y = y; 0 <= down_y - 1; down_y--)
                { // down_y 부터 바로 위의 값을 하나씩 당겨온다.
                    board_[x][down_y] = board_[x][down_y - 1];
                    board_[x][down_y - 1] = false;
                }
            }
            lines_left_--;
            y++;
        }
    }
}

void Game::draw_border()
{
    console::drawBox(0, 0, BOARD_WIDTH + 1, BOARD_HEIGHT + 1); // board 박스

    console::drawBox(BOARD_WIDTH + 2, 0, BOARD_WIDTH + 7, 5); // Next 박스
    console::draw(BOARD_WIDTH + 3, 0, "Next");                // Next 텍스트

    console::drawBox(BOARD_WIDTH + 8, 0, BOARD_WIDTH + 13, 5); // Hold 박스
    console::draw(BOARD_WIDTH + 9, 0, "Hold");                 // Hold 텍스트
}
void Game::draw_lines_left()
{
    std::string lines_left_string; // lines_left 를 string 형으로 으로 변환해서 이 변수에 넣음
    if (lines_left_ <= 0)
    { // LINES 이상 지웠을 경우 0으오 표기함
        lines_left_string = std::to_string(0);
    }
    else
    {
        lines_left_string = std::to_string(lines_left_);
    }

    std::string lines_left_display;

    lines_left_display.append(lines_left_string);
    lines_left_display.append(" lines left");

    console::draw(0, BOARD_HEIGHT + 2, lines_left_display);
}
void Game::draw_time(int x, int y)
{
    auto cur_time = std::chrono::steady_clock::now(); // 현재 시간
    auto time_passed = cur_time - start_time_;        // 걸린 시간 == 현재 시간 - 시작 시간

    // 경과한 분
    int m = std::chrono::duration_cast<std::chrono::minutes>(time_passed).count();
    // 경과한 초
    int s = // (time_passed % 1분) 연산을 하여 초를 구함
        std::chrono::duration_cast<std::chrono::seconds>(time_passed % std::chrono::minutes(1))
            .count();
    // 경과한 밀리초
    int ms = // (time_passed & 1초) 연산을 하여 밀리초를 구함
        std::chrono::duration_cast<std::chrono::milliseconds>(time_passed % std::chrono::seconds(1))
            .count() /
        10; // 세 자릿수로 나와서 두 자릿수로 바꾸기 위해 마지막에 10으로 나누어줌

    std::ostringstream time_display; // sstream 을 이용해서 포맷팅
    time_display << std::setw(2) << std::setfill('0') << m << ":"
                 << std::setw(2) << s << "."
                 << std::setw(2) << ms;

    console::draw(x, y, time_display.str()); // 최종 결과 출력
}
void Game::draw_fixed_blocks()
{
    for (int x = 0; x < BOARD_WIDTH; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT; y++)
        {
            if (board_[x][y] == true)
            {
                console::draw(x + 1, y + 1, BLOCK_STRING);
            }
        }
    }
}
void Game::draw_next_tet_and_hold_tet()
{
    next_tet_.drawAt(BLOCK_STRING, //    3 / size() 의 값은 size() 가 2, 3, 4 일 때 각각 1, 1, 0 이다.
                     (BOARD_WIDTH + 3) + (3 / next_tet_.size()),
                     1 + (3 / next_tet_.size()));
    if (have_held_ == true)
    { // 게임 중 swap 기능을 처음 사용했을 때 이후에만 swap_tet 을 출력한다.
        hold_tet_.drawAt(BLOCK_STRING,
                         (BOARD_WIDTH + 9) + (3 / hold_tet_.size()),
                         1 + (3 / hold_tet_.size()));
    }
}
void Game::draw_cur_tet_and_shadow()
{
    // 쉐도우를 먼저 그려야 겹쳐도 블록이 쉐도우 대신 표시된다
    cur_tet_.drawAt(SHADOW_STRING, cur_tet_x_ + 1, shadow_y_ + 1);
    cur_tet_.drawAt(BLOCK_STRING, cur_tet_x_ + 1, cur_tet_y_ + 1);
}
void Game::draw_lost()
{
    console::draw(2, 10, "You Lost");
}
void Game::draw_win()
{
    console::draw(3, 10, "You Win");
    draw_time(2, 11);
}