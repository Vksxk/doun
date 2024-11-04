#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
POSITION sample_obj_next_position(void);

/* ================= control =================== */
int sys_clock = 0;        // system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };
KEY last_key = k_none;    // 마지막 방향키 저장
int last_key_time = 0;    // 마지막 키 입력 시간 저장

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
    .spice = 0,
    .spice_max = 0,
    .population = 0,
    .population_max = 0
};

OBJECT_SAMPLE obj = {
    .pos = {1, 1},
    .dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
    .repr = 'o',
    .speed = 300,
    .next_move_time = 300
};

/* ================= main() =================== */
int main(void) {
    srand((unsigned int)time(NULL));

    init();
    intro();
    display(resource, map, cursor, false);  // 초기 호출 시 is_selected를 false로 전달

    bool is_selected = false; // 선택 상태를 저장하는 변수

    while (1) {
        // loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
        KEY key = get_key();

        // 키 입력이 있으면 처리
        if (is_arrow_key(key)) {
            int current_time = sys_clock;

            // 마지막 방향키와 현재 방향키가 같고, 시간 차가 짧다면 5칸 이동
            if (key == last_key && (current_time - last_key_time) < 200) {
                cursor_move(ktod(key)); // 첫 1칸 이동
                cursor_move(ktod(key));
                cursor_move(ktod(key));
                cursor_move(ktod(key));
            }
            else {
                cursor_move(ktod(key)); // 일반 1칸 이동
            }

            last_key = key;
            last_key_time = current_time; // 마지막 입력 시간 갱신
        }
        else {
            switch (key) {
            case k_quit:
                outro();
                break;
            case k_space: // 스페이스바 입력 처리
                is_selected = true; // 선택 상태로 변경
                break;
            case k_esc: // ESC키 입력 처리
                is_selected = false; // 선택 취소
                break;
            case k_none:
            case k_undef:
            default:
                break;
            }
        }

        // 샘플 오브젝트 동작
        sample_obj_move();

        // 화면 출력
        display(resource, map, cursor, is_selected);
        Sleep(TICK);
        sys_clock += 10;
    }

    return 0;
}

/* ================= subfunctions =================== */
void intro(void) {
    printf("DUNE 1.5\n");
    printf("이야 료이키텐카이");
    Sleep(2000);
    system("cls");
}

void outro(void) {
    printf("exiting...\n");
    printf("무령공처");
    exit(0);
}

void init(void) {
    // layer 0(map[0])에 지형 생성
    for (int j = 0; j < MAP_WIDTH; j++) {
        map[0][0][j] = '#';
        map[0][MAP_HEIGHT - 1][j] = '#';
    }

    for (int i = 1; i < MAP_HEIGHT - 1; i++) {
        map[0][i][0] = '#';
        map[0][i][MAP_WIDTH - 1] = '#';
        for (int j = 1; j < MAP_WIDTH - 1; j++) {
            map[0][i][j] = ' ';
        }
    }
    map[0][MAP_HEIGHT - 3][3] = 'P';
    map[0][MAP_HEIGHT - 3][4] = 'P';
    map[0][MAP_HEIGHT - 2][3] = 'P';
    map[0][MAP_HEIGHT - 2][4] = 'P';
    map[0][1][MAP_WIDTH - 4] = 'P';
    map[0][2][MAP_WIDTH - 4] = 'P';
    map[0][1][MAP_WIDTH - 5] = 'P';
    map[0][2][MAP_WIDTH - 5] = 'P';

    map[0][MAP_HEIGHT - 3][1] = 'B';
    map[0][MAP_HEIGHT - 3][2] = 'B';
    map[0][MAP_HEIGHT - 2][1] = 'B';
    map[0][MAP_HEIGHT - 2][2] = 'B';
    map[0][1][MAP_WIDTH - 3] = 'B';
    map[0][2][MAP_WIDTH - 3] = 'B';
    map[0][1][MAP_WIDTH - 2] = 'B';
    map[0][2][MAP_WIDTH - 2] = 'B';

    map[0][MAP_HEIGHT - 6][1] = 'S';
    map[0][5][MAP_WIDTH - 2] = 'S';

    map[0][MAP_HEIGHT - 6][7] = 'R';
    map[0][MAP_HEIGHT - 13][40] = 'R';
    map[0][5][MAP_WIDTH - 29] = 'R';
    map[0][5][MAP_WIDTH - 30] = 'R';
    map[0][4][MAP_WIDTH - 29] = 'R';
    map[0][4][MAP_WIDTH - 30] = 'R';
    map[0][MAP_HEIGHT - 4][MAP_WIDTH - 22] = 'R';
    map[0][MAP_HEIGHT - 4][MAP_WIDTH - 23] = 'R';
    map[0][MAP_HEIGHT - 5][MAP_WIDTH - 22] = 'R';
    map[0][MAP_HEIGHT - 5][MAP_WIDTH - 23] = 'R';
    map[0][MAP_HEIGHT - 3][MAP_WIDTH - 13] = 'R';

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            map[1][i][j] = -1;
        }
    }
    map[1][3][MAP_WIDTH - 2] = 'H';
    map[1][MAP_HEIGHT - 4][1] = 'H';

    map[1][3][12] = 'W';
    map[1][MAP_HEIGHT - 8][MAP_WIDTH - 15] = 'W';

    map[1][obj.pos.row][obj.pos.column] = 'o';
}

// (가능하다면) 지정한 방향으로 커서 이동
void cursor_move(DIRECTION dir) {
    POSITION curr = cursor.current;
    POSITION new_pos = pmove(curr, dir);

    // validation check
    if (1 <= new_pos.row && new_pos.row <= MAP_HEIGHT - 2 && \
        1 <= new_pos.column && new_pos.column <= MAP_WIDTH - 2) {

        cursor.previous = cursor.current;
        cursor.current = new_pos;
    }
}

/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
    POSITION diff = psub(obj.dest, obj.pos);
    DIRECTION dir;

    if (diff.row == 0 && diff.column == 0) {
        if (obj.dest.row == 1 && obj.dest.column == 1) {
            POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
            obj.dest = new_dest;
        }
        else {
            POSITION new_dest = { 1, 1 };
            obj.dest = new_dest;
        }
        return obj.pos;
    }

    if (abs(diff.row) >= abs(diff.column)) {
        dir = (diff.row >= 0) ? d_down : d_up;
    }
    else {
        dir = (diff.column >= 0) ? d_right : d_left;
    }

    POSITION next_pos = pmove(obj.pos, dir);
    if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
        1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
        map[1][next_pos.row][next_pos.column] < 0) {

        return next_pos;
    }
    else {
        return obj.pos;
    }
}

void sample_obj_move(void) {
    if (sys_clock <= obj.next_move_time) {
        return;
    }

    map[1][obj.pos.row][obj.pos.column] = -1;
    obj.pos = sample_obj_next_position();
    map[1][obj.pos.row][obj.pos.column] = obj.repr;

    obj.next_move_time = sys_clock + obj.speed;
}
