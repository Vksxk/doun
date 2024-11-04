/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"

// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 1, MAP_WIDTH + 2 }; // 상태창 위치 정의

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);
void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, bool is_selected);

// display.c
void display(
    RESOURCE resource,
    char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
    CURSOR cursor,
    bool is_selected // 추가된 인자
) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
    display_object_info(map, cursor, is_selected); // 수정된 부분
    //display_system_message();
    //display_commands();
}


void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// 오브젝트 정보를 표시하는 함수 정의
void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, bool is_selected) {
    POSITION curr = cursor.current; // 현재 커서 위치

    // 오브젝트와 유닛 구분하여 확인
    char current_object = map[0][curr.row][curr.column]; // 오브젝트는 첫 번째 레이어
    char current_unit = map[1][curr.row][curr.column];   // 유닛은 두 번째 레이어

    // 상태창 초기화
    gotoxy(status_pos);
    if (is_selected) { // 선택된 상태일 때만 출력
        printf("상태: "); // 줄바꿈 없이 상태 표시 시작

        // 유닛 확인 (현재 유닛이 있는 경우)
        switch (current_unit) {
        case 'H':
            printf("하베스터");
            break;
        case 'W':
            printf("  샌드웜");
            break;
        default:
            // 유닛이 없을 경우 오브젝트 정보를 출력
            switch (current_object) {
            case 'B':
                printf("    본진");
                break;
            case 'P':
                printf("    장판");
                break;
            case 'S':
                printf("스파이스");
                break;
            case 'R':
                printf("    바위");
                break;
            default:
                printf("사막 지형");
                break;
            }
            break;
        }
    }
    else {
        // 상태창 비우기: 상태창의 폭만큼 공백 출력
        printf("                    "); // 상태창 폭에 맞춰 충분한 공백 출력
    }
}




// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
    project(map, backbuf);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            // 기본 색상을 설정
            int color = COLOR_DEFAULT; // 기본 색상

            // 위치에 따른 색상 지정
            char current_tile = backbuf[i][j];

            if (current_tile == 'H') {
                if (i < MAP_HEIGHT / 2) {
                    color = AI_BASE; // AI 지역
                }
                else {
                    color = PLYAER_BASE; // 플레이어 지역
                }
            }
            else if (current_tile == 'B') {
                if (i < MAP_HEIGHT / 2) {
                    color = AI_BASE; // AI 지역
                }
                else {
                    color = PLYAER_BASE; // 플레이어 지역
                }
            }
            else if (current_tile == 'P') {
                color = TILE; // 일반 타일 색상
            }
            else if (current_tile == 'W') {
                color = SAND; // 모래 색상
            }
            else if (current_tile == 'S') {
                color = SPICE; // 향신료 색상
            }
            else if (current_tile == ' ') {
                color = OTHER; // 다른 타일 색상
            }

            // 화면에 출력
            if (frontbuf[i][j] != backbuf[i][j]) {
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], color); // 지정된 색상으로 출력
            }
            frontbuf[i][j] = backbuf[i][j]; // 현재 타일의 상태를 저장
        }
    }
}

void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // 이전 위치의 문자(ch)를 기본 색으로 출력
    char prev_ch = frontbuf[prev.row][prev.column];
    int prev_color = COLOR_DEFAULT; // 기본 색상으로 설정

    // 이전 위치의 문자가 무엇인지에 따라 색상 결정
    if (prev_ch == 'H') {
        prev_color = (prev.row < MAP_HEIGHT / 2) ? AI_BASE : PLYAER_BASE;
    }
    else if (prev_ch == 'B') {
        prev_color = (prev.row < MAP_HEIGHT / 2) ? AI_BASE : PLYAER_BASE;
    }
    else if (prev_ch == 'P') {
        prev_color = TILE;
    }
    else if (prev_ch == 'W') {
        prev_color = SAND;
    }
    else if (prev_ch == 'S') {
        prev_color = SPICE;
    }
    else if (prev_ch == ' ') {
        prev_color = OTHER;
    }

    // 이전 위치를 해당 색상으로 출력
    printc(padd(map_pos, prev), prev_ch, prev_color);

    // 현재 위치의 문자(ch)도 색상에 맞게 출력
    char curr_ch = frontbuf[curr.row][curr.column];
    int curr_color = COLOR_DEFAULT; // 기본 색상으로 설정

    if (curr_ch == 'H' || curr_ch == 'B') {
        curr_color = (curr.row < MAP_HEIGHT / 2) ? AI_BASE : PLYAER_BASE;
    }
    else if (curr_ch == 'P') {
        curr_color = TILE;
    }
    else if (curr_ch == 'W') {
        curr_color = SAND;
    }
    else if (curr_ch == 'S') {
        curr_color = SPICE;
    }
    else if (curr_ch == ' ') {
        curr_color = OTHER;
    }

    // 현재 위치를 해당 색상으로 출력
    printc(padd(map_pos, curr), curr_ch, curr_color);

    // 커서의 색상을 설정
    set_color(COLOR_CURSOR);
    gotoxy(padd(map_pos, curr)); // 커서 위치로 이동

    // 커서 표시 (투명하게 보이도록)
    printf(" "); // 커서 대신 공백으로 표시

    // 커서 색상 기본으로 되돌리기
    set_color(COLOR_DEFAULT);
}
