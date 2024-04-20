
#ifndef GAME_H
#define GAME_H

#include "tetromino.h"
#include <chrono>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define LINES 40

#define DROP_DELAY 60

class Game
{
private:
  // 게임 판을 나타내는 배열
  // board[x][y]가 true 인 경우 x, y 위치에 고정된 블록이 존재하는 것을 의미한다
  bool board_[BOARD_WIDTH][BOARD_HEIGHT];

  // 시작 시점을 저장하는 변수
  const std::chrono::time_point<std::chrono::steady_clock> start_time_;
  // 시작 시점에서 몇 초가 흘렀는지 저장하는 변수, 1초가 증가할 때마다 cur_tet_ 를 한 칸 내린다.
  int seconds_taken_;

  // 지워야 하는 라인 수, 지운 라인 수를 여기서 빼므로 음수가 될 수 있다.
  int lines_left_;
  // 게임의 상태를 나타내는 변수, -2 == 강제 종료, -1 == 패배, 0 == 진행 중, 1 == 승리
  int state_of_game_;

  // 현재 떨어지고 있는 테트로미노
  Tetromino cur_tet_;
  // 다음에 떨어질 테트로미노
  Tetromino next_tet_;
  // hold 한 테트로미노
  Tetromino hold_tet_;

  // 현재 떨어지고 있는 테트로미노의 좌측 상단 기준 x 좌표, board_ 배열의 인덱스를 넘어가는 값을 가질 수 있다.
  int cur_tet_x_;
  // 현재 떨어지고 있는 테트로미노의 좌측 상단 기준 y 좌표
  int cur_tet_y_;
  // 현재 떨어지고 있는 테트로미노의 쉐도우의 좌측 상단 기준 y 좌표
  int shadow_y_;

  // cur_tet 이 하드 드롭되어 더 이상 상태를 바꿀 수 없는가?
  bool fixed_cur_;

  // 게임 중 한 번이라도 hold 하였는가?
  bool have_held_;
  // cur_tet_ 이 완전히 떨어지기 전에는 hold 를 한 번 밖에 할 수 없다.
  bool can_hold_;

  // 랜덤한 테트로미노 만들어서 반환
  Tetromino make_rand_tet();

  // 키보드 입력을 받고 계산한다.
  void handle_input();

  // 그림자의 y값을 계산한다.
  void calculate_shadow_y();

  // 왼쪽 이동이 가능한지 계산, cur_tet_.check() 와 board_ 배열을 비교해 본다.
  bool can_left();
  // 오른쪽 이동이 가능한지 계산, cur_tet_.check() 와 board_ 배열을 비교해 본다.
  bool can_right();
  // 소프트 드롭이 가능한지 계산, cur_tet_.check() 와 board_ 배열을 비교해 본다.
  bool can_soft();
  // 회전이 가능한지 계산, 파라미터는 입력된 방향으로 회전된 테트로미노
  bool can_rotate(Tetromino rotated_tet);

  // new_tet() 할 수 있는지 bool 값을 반환하는 함수(hold 기능을 이용할 때도 사용한다).
  // 파라미터는 새로 판 안에 생성할 블록
  bool can_new_tet(Tetromino new_tet);
  // cur_tet_ 에 파라미터로 주어진 테트로미노를 집어 넣는다 (함수 실행 이후 new_tet 자리를 갱신해야 함)
  void new_tet(Tetromino new_tet);
  // hold 기능 구현, have_held_ 를 체크하여 동작이 달라지도록 한다.
  // 맨 처음에는 hold_tet_ 에 cur_tet_ 을 집어넣고, 두 번째부터는 hold_tet_ 과 cur_tet_ 을 교환한다.
  void hold_tet();

  // cur_tet_ 을 board_ 안에 집어넣어야 하는가?(다 떨어졌는지 cur_tet_y_ == shadow_y_ 로 검사)
  bool should_cur_to_board();
  // 다 떨어진 cur_tet_ 를 board_ 로 넣는다.
  void cur_to_board();
  // board_ 에서 가로로 꽉 찬 줄 삭제, 삭제한 만큼 내리기
  void delete_lines();

  // 게임 중 변하지 않는 테두리 출력
  void draw_border();
  // 남은 라인 텍스트 그리기
  void draw_lines_left();
  // 경과된 시간 그리기; 게임 실행 중, 게임 종료 시 출력; 파라미터는 그릴 x, y 좌표
  void draw_time(int x, int y);
  // 고정된 블록들을 그리기
  void draw_fixed_blocks();
  // Next, Hold 판 안의 테트로미노 그리기
  void draw_next_tet_and_hold_tet();
  // 떨어지고 있는 cur_tet, 쉐도우 그리기
  void draw_cur_tet_and_shadow();
  // 패배 화면 출력
  void draw_lost();
  // 승리 화면 출력
  void draw_win();

public:
  // 게임의 한 프레임을 처리한다.
  void update();

  // 게임 화면을 그린다.
  void draw();

  // 게임 루프가 종료되어야 하는지 여부를 반환한다.
  bool shouldExit();

  Game();
};
#endif
