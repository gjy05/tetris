#ifndef TETROMINO_H
#define TETROMINO_H

#include <string>

#define SHADOW_STRING "⛶"
#define BLOCK_STRING "■"

class Tetromino
{
  static const int MAX_SIZE = 4;

  // 디버그용 이름
  std::string name_;

  // 테트로미노의 모양을 저장하는 배열
  // shape[x][y]가 true인 경우 x, y 위치에 블록이 있는 것이고 false의 경우 없는
  // 것이다.
  bool shape_[MAX_SIZE][MAX_SIZE];

  // 회전되지 않은 원래 테트로미노 객체를 저장하는 포인터
  Tetromino *original_;

  // 테트로미노의 사이즈
  int size_;

public:
  // 사이즈와 shape 문자열을 통해 생성한다.
  // 문자열은 size * size 길이의 문자열이 주어진다.

  // 블록이 존재한다면 O를 통해 표시하고 아니라면 X를 통해 표시한다.
  // 문자열은 개행 없이 일렬로 나열되어 주어진다.
  // 예를 들어 T 미노의 모양과 생성은 다음과 같다.
  // XOX
  // OOO
  // XXX
  //
  // Tetromino T("T", 3, "XOXOOOXXX")

  // I 미노의 경우 다음과 같다.
  // XXXX
  // OOOO
  // XXXX
  // XXXX
  //
  // Tetromino I("I", 4, "XXXXOOOOXXXXXXXX")
  Tetromino(std::string name, int size, std::string shape);

  // 이름을 반환한다.
  std::string name() { return name_; }

  // 테트로미노의 사이즈를 반환한다.
  int size() { return size_; }

  // 회전되지 않은 원래 테트로미노 객체의 포인터를 반환한다.
  Tetromino *original() { return original_; }

  // 시계 방향으로 회전한 모습의 테트로미노 객체를 반환한다.
  Tetromino rotatedCW();

  // 반시계 방향으로 회전한 모습의 테트로미노 객체를 반환한다.
  Tetromino rotatedCCW();

  // 화면의 x, y 위치에 s 문자열로  테트로미노를 그린다
  void drawAt(std::string s, int x, int y);

  // 테트로미노의 좌상단 기준 x, y 위치에 블록이 있는지 여부를 나타내는 함수
  bool check(int x, int y) { return shape_[x][y]; }

  // 각 테트로미노 종류에 대한 선언
  // cpp 파일에서 Tetromino Tetromino::I(...); 와 같이 구현한다
  static Tetromino I, O, T, S, Z, J, L;
};

#endif
