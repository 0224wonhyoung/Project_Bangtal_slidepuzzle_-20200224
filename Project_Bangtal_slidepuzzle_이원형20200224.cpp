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

    Y 좌표는 실제 이미지 좌표와 방향이 다름.
*/

#include <Bangtal.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#pragma comment (lib, "Bangtal.lib")
using namespace std;

SceneID scene1;
ObjectID startButton;
ObjectID piece[16];

// arr : 4 X 4 퍼즐의 현재상태를 저장.
int arr[4][4];
// currentX[i] : 현재 i번쨰 조각의 X좌표가 어딘지
int currentX[16], currentY[16];
int dx[4] = { 1, 0, -1, 0 }, dy[4] = { 0, 1, 0, -1 };

// 이미지 파일의 위치
const char* imgFileLocate[16] = { "Images\\in1.jpg", "Images\\in2.jpg", "Images\\in3.jpg", "Images\\in4.jpg", "Images\\in5.jpg", "Images\\in6.jpg",
        "Images\\in7.jpg", "Images\\in8.jpg" , "Images\\in9.jpg" , "Images\\in10.jpg" , "Images\\in11.jpg" , "Images\\in12.jpg",
        "Images\\in13.jpg" , "Images\\in14.jpg" , "Images\\in15.jpg", "Images\\in16.jpg" };

// hideX,Y,Num.  현재 hide된 piece의 X,Y좌표와 몇번째인지
int hX = 0, hY = 0, hNum=0;

bool correct[16];

bool playing = false;
bool finish = false;


// for testing bug
void test() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", arr[j][i]);
        }
        printf("\n");
    }
}

int min(int a, int b) {
    return a > b ? b : a;
}

int myRand(int n) {
    int k = rand() % n;
    if (n > 0) {
        return k;
    }
    return 0;

}

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
    return (4 * y + x);
}
// 반댓방향 번호
int oppositeDirection(int d) {
    return ((d + 2) % 4);
}

// (x,y)좌표로부터 이미지상 X좌표.
// 사실 여기서 y는 안쓰이는 파라미터지만, coolX2 이런식으로 명명하면 훨씬 헷갈리니까
// 좌표값을 받는다는 형식이란걸 일단 이렇게 표기합시다.
int coolX(int x, int y) {
    return 340 + 150 * x;
}
int coolY(int x, int y) {
    return 510 - 150 * y;
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

// x+dx[direction], y+dy가 배열 안인지 확인
bool checkIn(int x, int y, int direction) {
    int tx = x + dx[direction], ty = y + dy[direction];
    return (tx > -1 && tx < 4 && ty > -1 && ty < 4);
}

// arr[4][4]를 초기화 + initObject가 true면 piece 위치도 초기화
void initializeArr(bool initObject) {
    int num = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            arr[j][i] = num;
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
// correct 초기화
void initializeCorrect() {
    for (int i = 0; i < 16; i++) {
        correct[i] = true;
    }
}
bool checkFin() {
    for (int i = 0; i < 16; i++) {
        if (!correct[i]) {
            return false;
        }
    }
    return true;
}

void updateCorrect(int x, int y) {
    int num = number(x, y);
    if (arr[x][y] == num) {
        correct[num] = true;
    }
    else {
        correct[num] = false;
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


void shuffle() {
    
    hNum = myRand(16);
    hX = arrayX(hNum);
    hY = arrayY(hNum);
    hideObject(piece[hNum]);
   

    // 128번 섞기
    for (int j = 0; j < 128; j++) {
        // directionIn[i] : 갈수있는 방향 중 i번째 방향
        int directionIn[4] = { 0, 0, 0, 0 };
        int n = 0, r = -1;


        // 갈수있는 방향 갯수 n 구하기 + 갈수있는 방향들에 번호부여
        for (int i = 0; i < 4; i++) {
            if (checkIn(hX, hY, i)) {
                n++;
                directionIn[n - 1] = i;
            }
        }

        r = myRand(n);
        //directionIn[r] 번째 방향으로 갈거임 ( = 그 방향 조각과 맞바꿀거임)

        int dNum = directionIn[r];

        int sx = hX + dx[dNum], sy = hY + dy[dNum];

        if (sx < 4) {
            int sNum = arr[sx][sy];

            int tx = hX, ty = hY;

            hX = sx, hY = sy;
            currentX[hNum] = sx, currentY[hNum] = sy;
            arr[sx][sy] = hNum;

            currentX[sNum] = tx, currentY[sNum] = ty;
            arr[tx][ty] = sNum;

            locateObject(piece[hNum], scene1, coolX(sx, 0), coolY(0, sy));
            locateObject(piece[sNum], scene1, coolX(tx, 0), coolY(0, ty));

            updateCorrect(sx, sy);
            updateCorrect(tx, ty);
        }

    }
}


void mouseCallback(ObjectID object, int x, int y, MouseAction) {
    if (object == startButton) {
        hideObject(startButton);
        playing = true;
        shuffle();
    }
    else if(playing){
        // num : 클릭한 조각 번호
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

                //이미지 갱신
                locateObject(piece[hNum], scene1, coolX(cx, 0), coolY(0, cy));
                locateObject(piece[num], scene1, coolX(tx, 0), coolY(0, ty));

                updateCorrect(cx, cy);
                updateCorrect(tx, ty);

                finish = checkFin();
                if (finish) {
                    playing = false;
                    showObject(startButton);
                    showObject(piece[hNum]);
                }
            }
        }
        
    }
}


int main()
{
    setMouseCallback(mouseCallback);

    
    scene1 = createScene("틀린그림찾기", "Images\\배경.jpg");

    // 조각 생성
    for (int i = 0; i < 16; i++) {
        piece[i] = createObject("조각", imgFileLocate[i]);
        locateObject(piece[i], scene1, coolX(i), coolY(i));
        showObject(piece[i]);
    }

    startButton = createObject("시작버튼", "Images\\start.png");
    locateObject(startButton, scene1, 590, 80);
    showObject(startButton);

    initializeArr(false);
    initializeCurrentXY();
    initializeCorrect();
    
    /*
    srand((unsigned int)time(NULL));
    hNum = rand() % 15;
    hX = arrayX(hNum);
    hY = arrayY(hNum);
     
    hideObject(piece[hNum]);
    */
    //shuffle();
    //test();
    
    
    startGame(scene1);
    
}




