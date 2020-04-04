/*  20200224 이원형
    4주차 과제

    배열 넘버링 방식

    0  1  2  3
    4  5  6  7
    8  9  10 11
    12 13 14 15
    
    (x,y)
    (0,0) (1,0) (2,0) (3,0)
    (0,1) (1,1) (2,1) (3,1)
    (0,2) (1,2) (2,2) (3,2)
    (0,3) (1,3) (2,3) (3,3)

    오른쪽 : 0번, (+1, 0)
    아래 : 1번, (0, +1)
    왼쪽 : 2번, (-1, 0)
    위 : 3번, (0, -1)
*/

#include <Bangtal.h>
#include <string>
#pragma comment (lib, "Bangtal.lib")

SceneID scene1;
ObjectID piece[16];

// arr : 4 X 4 퍼즐의 현재상태를 저장.
int arr[4][4];
// currentX[i] : 현재 i번쨰 조각의 X좌표가 어딘지
int currentX[16], currentY[16];
int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

// 이미지 파일의 위치
const char* imgFileLocate[16] = { "Images\\1.png", "Images\\2.png", "Images\\3.png", "Images\\4.png", "Images\\5.png", "Images\\6.png",
        "Images\\7.png", "Images\\8.png" , "Images\\9.png" , "Images\\10.png" , "Images\\11.png" , "Images\\12.png",
        "Images\\13.png" , "Images\\14.png" , "Images\\15.png", "Images\\16.png" };

// hideX,Y,Num.  현재 hide된 piece의 X,Y좌표와 몇번째인지
int hX = 0, hY = 0, hNum=0;





// num 번째 조각에 배열에서의 원래 위치의 X좌표
int arrayX(int num) {
    return num % 4;
}
// num 번째 조각에 배열에서의 원래 위치의 X좌표
int arrayY(int num) {
    return (int)(num / 4);
}
// 배열에서 x,y 좌표는 원래 number번째 조각.
int number(int x, int y) {
    return 4 * y + x;
}


// (x,y)좌표로부터 이미지상 X좌표.
// 사실 여기서 y는 안쓰이는 파라미터지만, coolX2 이런식으로 명명하면 훨씬 헷갈리니까
// 좌표값을 받는다는 형식이란걸 일단 이렇게 표기합시다.
int coolX(int x, int y) {
    return 305 + 150 * x;
}
int coolY(int x, int y) {
    return 460 - 150 * y;
}
// num 번쨰 조각의 원래 위치의 이미지상 X좌표
int coolX(int num) {
    return coolX(arrayX(num), 0);
}
// num 번쨰 조각의 원래 위치의 이미지상 Y좌표.
// 주의! 이미지 좌표는 자체정의한 배열의 Y좌표 방향과 반대.
int coolY(int num) {
    return coolY(0, arrayY(num));
}

// 마우스 입력을 받았을때 쓰일 함수.
// 클릭된 object가 몇번째 piece인지 return
int findNum(ObjectID object) {
    for (int i = 0; i < 16; i++) {
        if (piece[i] == object) {
            return i;
        }
    }
    return -1; // piece가 아닌 object가 들어오면 -1.
}

// arr[4][4]를 초기화 + initObject가 true면 piece 위치도 초기화
void initializeArr(bool initObject) {
    int num = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            arr[i][j] = num;
            if (initObject) {
                locateObject(piece[num], scene1, coolX(num), coolY(num));
            }
            num++;
        }
    }
}
// currentX,Y 초기화
void initializeCurrentXY() {
    for (int i = 0; i < 16; i++) {
        currentX[i] = arrayX(i);
        currentY[i] = arrayY(i);
    }
}

// hPiece와 붙어있는지 확인하고, 근처에 있으면 방향 번호 리턴
int checkHPieceDirection(int x, int y) {
    int tx = 0, ty = 0;
    for (int i = 0; i < 4; i++) {
        tx = x + dx[i];
        ty = y + dy[i];
        if (tx == hX && ty == hY) {
            return i;  // hidePiece가 근처에 있으면 방향 번호 리턴
        }
    }
    return -1; // hidePiece가 근처에 없으면 -1
}




void mouseCallback(ObjectID object, int x, int y, MouseAction) {
    
    int num = findNum(object);
    // piece가 아닌걸 클릭하면 -1 반환했으니, piece중 하나 클릭한 경우만
    if (num >= 0) {
        int cx = currentX[num], cy = currentY[num];
        int direction = checkHPieceDirection(cx, cy);
        // 만약 인접해있다면, 자리 맞바꾸기
        if (direction > -1) {
            int tx = hX, ty = hY;

            //hPiece 옮기기
            hX = cx, hY = cy;
            currentX[hNum] = cx, currentY[hNum] = cy;
            arr[cx][cy] = hNum;

            //클릭한 piece 옮기기
            currentX[num] = tx, currentY[num] = ty;
            arr[tx][ty] = num;

            locateObject(piece[hNum], scene1, coolX(num), coolY(num));
            locateObject(piece[num], scene1, coolX(cx + dx[direction], 0), coolY(0, cy + dy[direction]));
            
        }
    }
    }

int main()
{
    setMouseCallback(mouseCallback);

    
    scene1 = createScene("틀린그림찾기", "Images\\TEST.png");

    // 조각 생성
    for (int i = 0; i < 16; i++) {
        piece[i] = createObject("조각", imgFileLocate[i]);
        locateObject(piece[i], scene1, coolX(i), coolY(i));
        showObject(piece[i]);
    }

    initializeArr(false);
    initializeCurrentXY();


    hX = 0, hY = 0, hNum = 0; // TODO 아직 셔플기능 안넣었으므로 임의지정
    hideObject(piece[hNum]);

    
    startGame(scene1);
    
}




