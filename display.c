/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"

// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };
const POSITION status_pos = { 1, MAP_WIDTH + 2 }; // ����â ��ġ ����

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
    bool is_selected // �߰��� ����
) {
    display_resource(resource);
    display_map(map);
    display_cursor(cursor);
    display_object_info(map, cursor, is_selected); // ������ �κ�
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

// ������Ʈ ������ ǥ���ϴ� �Լ� ����
void display_object_info(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH], CURSOR cursor, bool is_selected) {
    POSITION curr = cursor.current; // ���� Ŀ�� ��ġ

    // ������Ʈ�� ���� �����Ͽ� Ȯ��
    char current_object = map[0][curr.row][curr.column]; // ������Ʈ�� ù ��° ���̾�
    char current_unit = map[1][curr.row][curr.column];   // ������ �� ��° ���̾�

    // ����â �ʱ�ȭ
    gotoxy(status_pos);
    if (is_selected) { // ���õ� ������ ���� ���
        printf("����: "); // �ٹٲ� ���� ���� ǥ�� ����

        // ���� Ȯ�� (���� ������ �ִ� ���)
        switch (current_unit) {
        case 'H':
            printf("�Ϻ�����");
            break;
        case 'W':
            printf("  �����");
            break;
        default:
            // ������ ���� ��� ������Ʈ ������ ���
            switch (current_object) {
            case 'B':
                printf("    ����");
                break;
            case 'P':
                printf("    ����");
                break;
            case 'S':
                printf("�����̽�");
                break;
            case 'R':
                printf("    ����");
                break;
            default:
                printf("�縷 ����");
                break;
            }
            break;
        }
    }
    else {
        // ����â ����: ����â�� ����ŭ ���� ���
        printf("                    "); // ����â ���� ���� ����� ���� ���
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
            // �⺻ ������ ����
            int color = COLOR_DEFAULT; // �⺻ ����

            // ��ġ�� ���� ���� ����
            char current_tile = backbuf[i][j];

            if (current_tile == 'H') {
                if (i < MAP_HEIGHT / 2) {
                    color = AI_BASE; // AI ����
                }
                else {
                    color = PLYAER_BASE; // �÷��̾� ����
                }
            }
            else if (current_tile == 'B') {
                if (i < MAP_HEIGHT / 2) {
                    color = AI_BASE; // AI ����
                }
                else {
                    color = PLYAER_BASE; // �÷��̾� ����
                }
            }
            else if (current_tile == 'P') {
                color = TILE; // �Ϲ� Ÿ�� ����
            }
            else if (current_tile == 'W') {
                color = SAND; // �� ����
            }
            else if (current_tile == 'S') {
                color = SPICE; // ��ŷ� ����
            }
            else if (current_tile == ' ') {
                color = OTHER; // �ٸ� Ÿ�� ����
            }

            // ȭ�鿡 ���
            if (frontbuf[i][j] != backbuf[i][j]) {
                POSITION pos = { i, j };
                printc(padd(map_pos, pos), backbuf[i][j], color); // ������ �������� ���
            }
            frontbuf[i][j] = backbuf[i][j]; // ���� Ÿ���� ���¸� ����
        }
    }
}

void display_cursor(CURSOR cursor) {
    POSITION prev = cursor.previous;
    POSITION curr = cursor.current;

    // ���� ��ġ�� ����(ch)�� �⺻ ������ ���
    char prev_ch = frontbuf[prev.row][prev.column];
    int prev_color = COLOR_DEFAULT; // �⺻ �������� ����

    // ���� ��ġ�� ���ڰ� ���������� ���� ���� ����
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

    // ���� ��ġ�� �ش� �������� ���
    printc(padd(map_pos, prev), prev_ch, prev_color);

    // ���� ��ġ�� ����(ch)�� ���� �°� ���
    char curr_ch = frontbuf[curr.row][curr.column];
    int curr_color = COLOR_DEFAULT; // �⺻ �������� ����

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

    // ���� ��ġ�� �ش� �������� ���
    printc(padd(map_pos, curr), curr_ch, curr_color);

    // Ŀ���� ������ ����
    set_color(COLOR_CURSOR);
    gotoxy(padd(map_pos, curr)); // Ŀ�� ��ġ�� �̵�

    // Ŀ�� ǥ�� (�����ϰ� ���̵���)
    printf(" "); // Ŀ�� ��� �������� ǥ��

    // Ŀ�� ���� �⺻���� �ǵ�����
    set_color(COLOR_DEFAULT);
}
