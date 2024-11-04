// display.h
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "common.h"

// ���� ����
#define COLOR_DEFAULT	15
#define COLOR_CURSOR	112
#define COLOR_RESOURCE  112
#define PLYAER_BASE 1
#define AI_BASE 4
#define SAND 6
#define TILE 8
#define SPICE 14
#define OTHER 7

// ����â ��ġ ����
extern const POSITION status_pos; // ����â�� ��ġ�� �ܺο��� ����� �� �ֵ��� ����

// ȭ�鿡 ǥ���� �����
void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor,
	bool is_selected // ����â�� ǥ������ ���θ� ��Ÿ���� ���� �߰�
);

#endif
