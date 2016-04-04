//
//  main.cpp
//  Snack Proj
//
//  Created by Runzhe Yang on 8/3/15.
//  Copyright (c) 2015 Runzhe Yang. All rights reserved.
//

#if(__cplusplus == 201103L)
#include <unordered_map>
#include <unordered_set>
#else
#include <tr1/unordered_map>
#include <tr1/unordered_set>
namespace std
{
    using std::tr1::unordered_map;
    using std::tr1::unordered_set;
}
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <ctime>
#include <climits>
#include <map>
#include <algorithm>
#include <fstream>
#include <vector>
#include "jsoncpp/json.h"

#define PSS pair<string, string>

using namespace std;

int n, m;
const int maxn = 19;
const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};
const long long err = 1e-7;
const long long INF = LONG_LONG_MAX;
bool invalid[maxn][maxn];

const int MYCONSIDER = 40;
const int ENECONSIDER = 14;

int SearchDepth = 6;

const int SearchDepthSup = 12;
const int STATUS_SUP = 40000;

const int DEF = 4;
const int ATA = 6;
const int BAL = 3;

struct point
{
    int x, y;
    point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
};

list<point> snake[2]; // 0表示自己的蛇，1表示对方的蛇
int possibleDire[10];
int posCount = 0;

inline bool whetherGrow(const int &num)  //本回合是否生长
{
    if (num <= 9)
        return true;
    if ((num - 9) % 3 == 0)
        return true;
    return false;
}

inline void deleteEnd(const int &id)     //删除蛇尾
{
    snake[id].pop_back();
}

inline void move(const int &id, const int &dire, const int &num)  //编号为id的蛇朝向dire方向移动一步
{
    point p = *(snake[id].begin());
    int x = p.x + dx[dire];
    int y = p.y + dy[dire];
    snake[id].push_front(point(x, y));
    if (!whetherGrow(num))
        deleteEnd(id);
}

inline void outputSnakeBody(const int &id)    //调试语句
{
    cout << "Snake No." << id << endl;
    for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
        cout << iter->x << " " << iter->y << endl;
    cout << endl;
}

inline bool isInBody(const int &x, const int &y)   //判断(x,y)位置是否有蛇
{
    for (int id = 0; id <= 1; ++id)
        for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
            if (x == iter->x && y == iter->y)
                return true;
    return false;
}

inline bool validDirection(const int &id, const int &k)  //判断当前移动方向的下一格是否合法
{
    point p = *(snake[id].begin());
    int x = p.x + dx[k];
    int y = p.y + dy[k];
    if (x > n || y > m || x < 1 || y < 1)
        return false;
    if (invalid[x][y])
        return false;
    if (isInBody(x, y))
        return false;
    return true;
}

int Rand(int p)   //随机生成一个0到p的数字
{
    return rand() * rand() * rand() % p;
}







/////////////////////////////// MY STRATEGY //////////////////////////////////////////

int visMap[maxn][maxn] = {0};
int decisionList[10];
int decisionCnt = 0;
queue<point> Q;
queue<point> E;
int startTotal = 0, fakeTotal = 0;
list<point> fakeSnake[2];

bool inBodyBoost[maxn][maxn] = {0};

 //复原两条蛇
inline void initFakeAll(const int &total)
{
    startTotal = fakeTotal = total;
    for (int id = 0;id <= 1; ++id)
        for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
            fakeSnake[id].push_back(point(iter->x, iter->y));
}

//调试信息
inline void outputFakeSnakeBody(const int &id)    //调试语句
{
    cout << "Snake No." << id << endl;
    for (list<point>::iterator iter = fakeSnake[id].begin(); iter != fakeSnake[id].end(); ++iter)
        cout << iter->x << " " << iter->y << endl;
    cout << endl;
}

//判断(x,y)位置是否有蛇
inline bool fakeIsInBody(const int &x, const int &y)
{
    for (int id = 0; id <= 1; ++id)
        for (list<point>::iterator iter = fakeSnake[id].begin(); iter != fakeSnake[id].end(); ++iter)
            if (x == iter->x && y == iter->y)
                return true;
    return false;
}

//判断当前移动方向的下一格是否合法
inline bool fakeValidDirection(const int &id, const int &k, const bool &canBoost)
{
    point p = *(fakeSnake[id].begin());
    int x = p.x + dx[k];
    int y = p.y + dy[k];
    
    if (!canBoost) {
        memset(inBodyBoost, 0, sizeof(inBodyBoost));
        for (int id = 0; id <= 1; ++id)
            for (list<point>::iterator iter = fakeSnake[id].begin(); iter != fakeSnake[id].end(); ++iter)
                inBodyBoost[iter->x][iter->y] = true;
    }
    
    if (x > n || y > m || x < 1 || y < 1)
        return false;
    
    
    if (invalid[x][y])
        return false;
    
    if (inBodyBoost[x][y])
        return false;
    
    return true;
}

//编号为id的蛇朝向dire方向移动一步,不处理蛇尾
inline void fakeMove(const int &id, const int &dire)
{
    point p = *(fakeSnake[id].begin());
    int x = p.x + dx[dire];
    int y = p.y + dy[dire];
    fakeSnake[id].push_front(point(x, y));
}

//删除蛇尾
inline void fakeDeleteEnd(const int &id)
{
    fakeSnake[id].pop_back();
}

//恢复蛇头
inline void fakeBackOneStep(const int &id)
{
    fakeSnake[id].pop_front();
}

//复原某条蛇
inline void initSnake(const int &id)
{
    fakeSnake[id].clear();
    for (list<point>::iterator iter = snake[id].begin(); iter != snake[id].end(); ++iter)
        fakeSnake[id].push_back(point(iter->x, iter->y));
}

//判断当前移动方向的下一格是否合法
inline bool validTry(const point &p, const int &k, const int &id, const bool &canBoost)
{
    int x = p.x + dx[k];
    int y = p.y + dy[k];
    
    if (!canBoost) {
        memset(inBodyBoost, 0, sizeof(inBodyBoost));
        for (int id = 0; id <= 1; ++id)
            for (list<point>::iterator iter = fakeSnake[id].begin(); iter != fakeSnake[id].end(); ++iter)
                inBodyBoost[iter->x][iter->y] = true;
    }
    
    if (x > n || y > m || x < 1 || y < 1)
        return false;
    
    if (invalid[x][y])
        return false;
    
    if (inBodyBoost[x][y])
        return false;
    
    if (id == 0 && (visMap[x][y] == 1 || visMap[x][y] >= 3))
        return false;
    if (id == 1 && visMap[x][y])
        return false;
    return true;
}


//势力范围
void powerRegion(int &blankCnt, int &E_blankCnt) {
    memset(visMap, 0, sizeof(visMap));
    while (!Q.empty())
        Q.pop();
    while (!E.empty())
        E.pop();
    
    point p = *(fakeSnake[0].begin());
    visMap[p.x][p.y] = 1;
    Q.push(p);
    
    point enemy = *(fakeSnake[1].begin());
    visMap[enemy.x][enemy.y] = 1;
    E.push(enemy);
    
    int myStepLevelCnt = 0;
    int eneStepLevelCnt = 0;
    
    while (!Q.empty() || !E.empty()) {
        bool canBoost = false;
        int enemyStepCnt = int(E.size());
        while (!E.empty()) {
            point tmpE = E.front();
            E.pop();
            for (int k = 0; k < 4; ++k) {
                bool isValid = validTry(tmpE, k, 1, canBoost);
                canBoost = true;
                if (isValid && myStepLevelCnt <= MYCONSIDER) {
                    
                    if (eneStepLevelCnt <= ENECONSIDER)
                        E_blankCnt += ATA;
                    point new_add_E(tmpE.x + dx[k], tmpE.y + dy[k]);
                    visMap[new_add_E.x][new_add_E.y] = 2;
                    E.push(new_add_E);
                }
            }
            if (--enemyStepCnt == 0) {
                ++eneStepLevelCnt;
                break;
            }
        }
        
        int myStepCnt = int(Q.size());
        while (!Q.empty()) {
            point tmp = Q.front();
            Q.pop();
            for (int k = 0; k < 4; ++k) {
                bool isValid = validTry(tmp, k, 0, canBoost);
                canBoost = true;
                if (isValid && myStepLevelCnt <= MYCONSIDER) {
                    if (myStepLevelCnt <= MYCONSIDER)
                        blankCnt += DEF;
                    point new_add(tmp.x + dx[k], tmp.y + dy[k]);
                    if (visMap[new_add.x][new_add.y] == 2) {
                        if (myStepLevelCnt <= MYCONSIDER)
                            blankCnt -= BAL;
                    }
                    visMap[new_add.x][new_add.y]++;
                    Q.push(new_add);
                }
            }
            if (--myStepCnt == 0) {
                ++myStepLevelCnt;
                break;
            }
        }
        
    }
}


bool cmp(PSS a, PSS b) {
    return a.first.length() > b.second.length()
    || (a.first.length() == b.second.length() && a.first > b.first)
    || (a.first.length() == b.second.length() && a.first == b.first && a.second > b.second);
}


inline bool deciSimilar(const string &a, const string &b) {
    if (a.length() < b.length() && a.length() > 1) {
        for (int i = 1; i < a.length(); ++i) {
            if (a[i] != b[i]) return false;
        }
        return true;
    }
    return a == b;
}


queue<PSS> simuDeci;
vector<PSS> waitForJudging;
deque<PSS> candidateH;
unordered_map<string, long long> Helper;

long long helpme() {
    int TOT = int(waitForJudging.size());
    int level = int(waitForJudging[0].first.size());
    int lastTimeCanTot = 0;
    
    for (int ind = 0; ind < TOT; ++ind) {
        string mySimuD = waitForJudging[ind].first;
        string eneSimuD = waitForJudging[ind].second;
        
        if (mySimuD.size() < level) {
            
            for (int i = 0; i < lastTimeCanTot; ++i) {
                PSS tmp;
                tmp.first = candidateH[i].first;
                tmp.second = candidateH[i].second.substr(0, candidateH[i].second.length() - 1);
                
                unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
                unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
                
                if (Iter != Helper.end()) {
                    if ((*candiIter).second < (*Iter).second) (*Iter).second = (*candiIter).second;
                } else {
                    Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                    candidateH.push_back(tmp);
                }
                Helper.erase((*candiIter).first);
            }
            for (int i = 0; i < lastTimeCanTot; ++i) {
                candidateH.pop_front();
            }
            
            
//cout << endl << "1_______BIG NEWS " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;:; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
            
            int candiTOT = int(candidateH.size());
            for (int i = 0; i < candiTOT; ++i) {
                PSS tmp;
                string str1, str2;
                str1 = candidateH[i].first;
                str2 = candidateH[i].second;
                tmp.first = str1.substr(0, str1.length() - 1);
                tmp.second = str2.substr();
                
                unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
                unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
                
                if (Iter != Helper.end()) {
                    if ((*candiIter).second > (*Iter).second) (*Iter).second = (*candiIter).second;
                } else {
                    Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                    candidateH.push_back(tmp);
                }
                Helper.erase((*candiIter).first);
                
            }
            for (int i = 0; i < candiTOT; ++i) {
                candidateH.pop_front();
            }
            
            lastTimeCanTot = int(candidateH.size());
            
            --level;
            
//cout << endl << "BG NEWS_______ " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;-; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
        }
        
        while (mySimuD.length() < level) {
            int candiTOT = int(candidateH.size());
            for (int i = 0; i < candiTOT; ++i) {
                PSS tmp;
                tmp.first = candidateH[i].first;
                tmp.second = candidateH[i].second.substr(0, candidateH[i].second.length() - 1);
                
                unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
                unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
                
                if (Iter != Helper.end()) {
                    if ((*candiIter).second < (*Iter).second) (*Iter).second = (*candiIter).second;
                } else {
                    Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                    candidateH.push_back(tmp);
                }
                Helper.erase((*candiIter).first);
            
            }
            for (int i = 0; i < candiTOT; ++i) {
                candidateH.pop_front();
            }

            
//cout << endl << "2_______BIG NEWS " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;:; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
            
            candiTOT = int(candidateH.size());
            for (int i = 0; i < candiTOT; ++i) {
                PSS tmp;
                string str1, str2;
                str1 = candidateH[i].first;
                str2 = candidateH[i].second;
                tmp.first = str1.substr(0, str1.length() - 1);
                tmp.second = str2.substr();
                
                unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
                unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
                
                if (Iter != Helper.end()) {
                    if ((*candiIter).second > (*Iter).second) (*Iter).second = (*candiIter).second;
                } else {
                    Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                    candidateH.push_back(tmp);
                }
                Helper.erase((*candiIter).first);
            
            }
            for (int i = 0; i < candiTOT; ++i) {
                candidateH.pop_front();
            }
            lastTimeCanTot = int(candidateH.size());
            --level;

//cout << endl << "BG NEWS_______ " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;-; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
        
        }
        
        for (int i = 0; i < mySimuD.length(); ++i) {
            if (i != 0 && !whetherGrow(startTotal + i)) {
                fakeDeleteEnd(0);
                fakeDeleteEnd(1);
            }
            fakeMove(0, mySimuD[i] - '0');
            fakeMove(1, eneSimuD[i] - '0');
        }
        
        int tmpBlankCnt = 0, tmpE_blankCnt = 0;
        powerRegion(tmpBlankCnt, tmpE_blankCnt);
        
        initSnake(0);
        initSnake(1);
        
        if (tmpE_blankCnt == 0) {
            tmpBlankCnt *= 1000;
            tmpE_blankCnt = 1;
        }
        
//cout << ":::: " << mySimuD << " " << eneSimuD << endl;
        
        long long tmpValue = ((1ll * tmpBlankCnt) << 18) / tmpE_blankCnt;
        PSS tmp;
        tmp.first = mySimuD;
        tmp.second = eneSimuD.substr(0, eneSimuD.length() - 1);
        
        unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
        
        if (Iter != Helper.end()) {
            if (tmpValue < (*Iter).second) (*Iter).second = tmpValue;
        } else {
            Helper.insert(make_pair(tmp.first + " " + tmp.second, tmpValue));
            candidateH.push_back(tmp);
        }
    }
    
    
    
    
    
    while (level > 1) {
        int candiTOT = int(candidateH.size());
        for (int i = 0; i < lastTimeCanTot; ++i) {
            PSS tmp;
            tmp.first = candidateH[i].first;
            tmp.second = candidateH[i].second.substr(0, candidateH[i].second.length() - 1);
            
            unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
            unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
            
            if (Iter != Helper.end()) {
                if ((*candiIter).second < (*Iter).second) (*Iter).second = (*candiIter).second;
            } else {
                Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                candidateH.push_back(tmp);
            }
            Helper.erase((*candiIter).first);
            
        }
        for (int i = 0; i < lastTimeCanTot; ++i) {
            candidateH.pop_front();
        }

        
//cout << endl << "3_______BIG NEWS " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;:; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
        
        candiTOT = int(candidateH.size());
        for (int i = 0; i < candiTOT; ++i) {
            PSS tmp;
            string str1, str2;
            str1 = candidateH[i].first;
            str2 = candidateH[i].second;
            tmp.first = str1.substr(0, str1.length() - 1);
            tmp.second = str2.substr();
            
            unordered_map<string, long long>::iterator Iter = Helper.find(tmp.first + " " + tmp.second);
            unordered_map<string, long long>::iterator candiIter = Helper.find(candidateH[i].first + " " + candidateH[i].second);
            
            if (Iter != Helper.end()) {
                if ((*candiIter).second > (*Iter).second) (*Iter).second = (*candiIter).second;
            } else {
                Helper.insert(make_pair(tmp.first + " " + tmp.second, (*candiIter).second));
                candidateH.push_back(tmp);
            }
            Helper.erase((*candiIter).first);
            
        }
        for (int i = 0; i < candiTOT; ++i) {
            candidateH.pop_front();
        }
        lastTimeCanTot = int(candidateH.size());
        --level;

//cout << endl << "BG NEWS_______ " << endl;
//for (int i = 0; i < candidateH.size(); ++i) {
//    cout << " :;:;-; " << candidateH[i].first << " " << candidateH[i].second << " " << Helper[candidateH[i].first + " " + candidateH[i].second] << endl;
//}
//cout << endl;
    
    }
    
    long long EsV = INF;
    int candiTOT = int(candidateH.size());
    for (int i = 0 ; i < candiTOT; ++i) {
        EsV = min(EsV, Helper[candidateH[i].first + " " + candidateH[i].second]);
    }
    
    candidateH.clear();
    Helper.clear();
    
    return EsV;
}

long long estimate(const int &nextStep, int &blankCnt, int &E_blankCnt)
{
    
    string mySimuD = " ", eneSimuD = " ";
    mySimuD[0] = nextStep + '0';
    fakeMove(0, nextStep);
    bool canBoost = false;
    for (int k = 0; k < 4; ++k) {
        bool isValid = fakeValidDirection(1, k, canBoost);
        canBoost = true;
        if (isValid) {
            eneSimuD[0] = k + '0';
            simuDeci.push(make_pair(mySimuD, eneSimuD));
        }
    }
    fakeBackOneStep(0);
    fakeTotal = startTotal;
    
    
    while (fakeTotal - startTotal + 1 < SearchDepth) {
        
        int ThisLevelSize = int(simuDeci.size());

        
        while (!simuDeci.empty()) {
            bool canBoost = false;
            bool terminated = false;
            PSS tmp = simuDeci.front();
            mySimuD = tmp.first;
            eneSimuD = tmp.second;
            
            if (mySimuD.length() <= fakeTotal - startTotal) {

                simuDeci.push(tmp);
                simuDeci.pop();
                if (--ThisLevelSize <= 0)
                    break;
                continue;
            }
        
            int counterI = 0;
            for (int i = 0; i < mySimuD.length(); ++i) {
                if (i != 0 && !whetherGrow(startTotal + i)) {
                    fakeDeleteEnd(0);
                    fakeDeleteEnd(1);
                }
                fakeMove(0, mySimuD[i] - '0');
                fakeMove(1, eneSimuD[i] - '0');
                counterI = i;
            }
            
            if (!whetherGrow(startTotal + counterI + 1)) {
                fakeDeleteEnd(0);
                fakeDeleteEnd(1);
            }
            
            for (int k = 0; k < 4; ++k) {
                bool isValid = fakeValidDirection(0, k, canBoost);
                canBoost = true;
                if (isValid) {
                    fakeMove(0, k);
                    canBoost = false;
                    for (int l = 0; l < 4; ++l) {
                        isValid = fakeValidDirection(1, l, canBoost);
                        canBoost = true;
                        if (isValid) {
                
                            string new_mySimuD = " ", new_eneSimuD = " ";
                            new_mySimuD[0] = k + '0';
                            new_eneSimuD[0] = l + '0';
                            new_mySimuD = mySimuD + new_mySimuD;
                            new_eneSimuD = eneSimuD + new_eneSimuD;
                            
                            simuDeci.push(make_pair(new_mySimuD, new_eneSimuD));
                            terminated = true;
                        }
                    }
                    
                    fakeBackOneStep(0);
                    canBoost = false;
                }
            }
            
            initSnake(0);
            initSnake(1);
            
            if (terminated) {
                simuDeci.pop();
            } else {
                simuDeci.pop();
                simuDeci.push(tmp);
            }
            
            if (--ThisLevelSize <= 0)
                break;
        }
        ++fakeTotal;
    }
    
    string lastMySimuD = " ";
    long long EsV = INF;
    
//cout << simuDeci.size() << endl;
    
    waitForJudging.clear();
    while (!simuDeci.empty()) {
        PSS tmp = simuDeci.front();
        simuDeci.pop();
        waitForJudging.push_back(tmp);
    }
    
    sort(waitForJudging.begin(), waitForJudging.end(), cmp);
    
    if (!waitForJudging.empty()) EsV = helpme();

    return EsV;
}



//最终决定
int myStrategy(const int &total)
{
    long long stepValue = -1;
    initFakeAll(total);
    
    for (int i = 0; i < posCount; ++i) {
        
        int blankCnt = 0;
        int E_blankCnt = 0;
        long long EsV = estimate(possibleDire[i], blankCnt, E_blankCnt);
        
//cout << "---------------------- oo " << possibleDire[i] << " " << EsV << endl;
        
        if (EsV < stepValue + err && EsV > stepValue - err) {
            decisionList[decisionCnt++] = i;
        }
        if (EsV > stepValue) {
            stepValue = EsV;
            decisionCnt = 0;
            decisionList[decisionCnt++] = i;
        }
    }
    
    if (decisionCnt == 0) return 0;
    return possibleDire[decisionList[rand() % decisionCnt]];
}

int deepRegion(const int &total)
{
    initFakeAll(total);
    string mySimuD = " ", eneSimuD = " ";
    for (int i = 0; i < posCount; ++i) {
        
        int nextStep = possibleDire[i];
        mySimuD[0] = nextStep + '0';
        fakeMove(0, nextStep);
        bool canBoost = false;
        for (int k = 0; k < 4; ++k) {
            bool isValid = fakeValidDirection(1, k, canBoost);
            canBoost = true;
            if (isValid) {
                eneSimuD[0] = k + '0';
                simuDeci.push(make_pair(mySimuD, eneSimuD));
            }
        }
        fakeBackOneStep(0);
    
    }
    
    fakeTotal = startTotal;
    
    while (fakeTotal - startTotal + 1 < SearchDepthSup) {
        int ThisLevelSize = int(simuDeci.size());
        
        while (!simuDeci.empty()) {
            bool canBoost = false;
            bool terminated = false;
            PSS tmp = simuDeci.front();
            mySimuD = tmp.first;
            eneSimuD = tmp.second;
        
            if (mySimuD.length() <= fakeTotal - startTotal) {
            
                simuDeci.push(tmp);
                simuDeci.pop();
                if (--ThisLevelSize <= 0)
                    break;
                continue;
            }
            
            int counterI = 0;
            for (int i = 0; i < mySimuD.length(); ++i) {
                if (i != 0 && !whetherGrow(startTotal + i)) {
                    fakeDeleteEnd(0);
                    fakeDeleteEnd(1);
                }
                fakeMove(0, mySimuD[i] - '0');
                fakeMove(1, eneSimuD[i] - '0');
                counterI = i;
            }
            
            if (!whetherGrow(startTotal + counterI + 1)) {
                fakeDeleteEnd(0);
                fakeDeleteEnd(1);
            }
            
            for (int k = 0; k < 4; ++k) {
                bool isValid = fakeValidDirection(0, k, canBoost);
                canBoost = true;
                if (isValid) {
                    fakeMove(0, k);
                    canBoost = false;
                    for (int l = 0; l < 4; ++l) {
                        isValid = fakeValidDirection(1, l, canBoost);
                        canBoost = true;
                        if (isValid) {
                        
                            string new_mySimuD = " ", new_eneSimuD = " ";
                            new_mySimuD[0] = k + '0';
                            new_eneSimuD[0] = l + '0';
                            new_mySimuD = mySimuD + new_mySimuD;
                            new_eneSimuD = eneSimuD + new_eneSimuD;
                            
                            simuDeci.push(make_pair(new_mySimuD, new_eneSimuD));
                            terminated = true;
                        }
                    }
                
                    fakeBackOneStep(0);
                    canBoost = false;
                }
            }
        
            initSnake(0);
            initSnake(1);
        
            if (terminated) {
                simuDeci.pop();
            } else {
                simuDeci.pop();
                simuDeci.push(tmp);
            }
        
            if (--ThisLevelSize <= 0)
                break;
        }
        ++fakeTotal;
        if (int(simuDeci.size()) > STATUS_SUP)
            break;
    }
    
    while (!simuDeci.empty()) {
        simuDeci.pop();
    }

    return fakeTotal - startTotal;
}

//////////////////////////////////////////////////////////////////////////////////////



int main(int argc, const char * argv[]) {
    
//clock_t start_time = clock();
    
    memset(invalid, 0, sizeof(invalid));
    string str;
    string temp;
//ifstream cin("/Users/YRZ/Desktop/inforSnake/in14.txt");
    while (getline(cin, temp))
        str += temp;
    
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    
    n = input["requests"][(Json::Value::UInt) 0]["height"].asInt();  //棋盘高度
    m = input["requests"][(Json::Value::UInt) 0]["width"].asInt();   //棋盘宽度
    
    int x = input["requests"][(Json::Value::UInt) 0]["x"].asInt();  //读蛇初始化的信息
    if (x == 1)
    {
        snake[0].push_front(point(1, 1));
        snake[1].push_front(point(n, m));
    }
    else
    {
        snake[1].push_front(point(1, 1));
        snake[0].push_front(point(n, m));
    }
    
    //处理地图中的障碍物
    int obsCount = input["requests"][(Json::Value::UInt) 0]["obstacle"].size();
    
    for (int i = 0; i < obsCount; ++i)
    {
        int ox = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["x"].asInt();
        int oy = input["requests"][(Json::Value::UInt) 0]["obstacle"][(Json::Value::UInt) i]["y"].asInt();
        invalid[ox][oy] = 1;
    }
    
    //根据历史信息恢复现场
    int total = input["responses"].size();
    
    int dire;
    for (int i = 0; i < total; ++i)
    {
        dire = input["responses"][i]["direction"].asInt();
        move(0, dire, i);
        
        dire = input["requests"][i + 1]["direction"].asInt();
        move(1, dire, i);
        
    }
    
    if (!whetherGrow(total)) // 本回合两条蛇生长
    {
        deleteEnd(0);
        deleteEnd(1);
    }
    
    
    for (int k = 0; k < 4; ++k)
        if (validDirection(0, k))
            possibleDire[posCount++] = k;
    
    srand((unsigned)time(0) + total);
    
    
    SearchDepth = deepRegion(total);
    
//cout << SearchDepth << endl;
    
    Json::Value ret;
    ret["response"]["direction"] = myStrategy(total);
    
    
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
    
    
//clock_t end_time = clock();
//cout << static_cast<double> (end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
    
    return 0;
}