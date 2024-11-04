// display.h
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// 색상 정의
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
#define PLYAER_BASE 1
#define AI_BASE 4
#define SAND 6
#define TILE 8
#define SPICE 14
#define OTHER 7

// 상태창 위치 정의
extern const POSITION status_pos; // 상태창의 위치를 외부에서 사용할 수 있도록 선언

// 화면에 표시할 내용들
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	bool is_selected // 상태창을 표시할지 여부를 나타내는 변수 추가
);

#endif
