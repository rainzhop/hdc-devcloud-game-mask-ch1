#include <bits/stdc++.h>

using namespace std;

#define C 100 // 快递车满载量

unsigned char orig_x, orig_y; // 仓库位置

unsigned char curr_x, curr_y; // 快递车现位置
short curr_q = C; // 快递车现载量

unsigned char dem_x[5], dem_y[5], dem_done_cnt; // 需求点
unsigned char don_x[138], don_y[138], don_cnt; // 捐赠点
short map_q[12][12] = {0, }; // 需求捐赠量地图
short total_dem_q = 0; // 总需求量
int kk = 0;

short route_x[500], route_y[500], route_len, route_pos; // 路径
int route_step_cnt = 0;

int disp_qmap()
{
    printf("curr_q = %d\n", curr_q);
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (i == orig_x && j == orig_y)
                printf(" inf ");
            else if (i == curr_x && j == curr_y)
                printf(" **  ");
            else
                printf("%4d ",map_q[i][j]);
        }
        printf("\n");
    }
    return 0;
}

class Node
{
public:
    set<Node>::iterator parent;
    unsigned char done;
    short step_cnt;
    short mapq[12][12];
    unsigned char cx, cy, cq;
    unsigned char dx, dy;
    unsigned char done_cnt; // 已完成节点数
    short done_q; // 已完成需求量
    short donate_flag;
    Node(set<Node>::iterator p, unsigned char x, unsigned char y);
    bool operator < (const Node &right) const;
    int sim_update();
    int sim_walk();
};

#define K 5.5

struct cmp
{
    bool operator () (const Node &left, const Node &right) const 
    {
        int left_sugar = left.step_cnt - left.done_cnt * K;
        int right_sugar = right.step_cnt - right.done_cnt * K;
        if (left_sugar != right_sugar) return left_sugar < right_sugar;
        else if (left.step_cnt != right.step_cnt) return left.step_cnt < right.step_cnt;
        else if (left.cq != right.cq) return left.cq > right.cq;
        else return left.done_q > right.done_q;
    }
}; 

set<Node> passed_nodes;
set<Node, cmp> passing_nodes;

bool Node::operator < (const Node &right) const
{   
    if (this->cq != right.cq) return this->cq < right.cq;
    else if (this->cy != right.cy) return this->cy < right.cy;
    else if (this->cx != right.cx) return this->cx < right.cx;
    else return memcmp(this->mapq, right.mapq, 288) < 0;
}

Node::Node(set<Node>::iterator p, unsigned char x, unsigned char y)
{
    parent = p; 
    if (p != passed_nodes.end())
    {
        memcpy(mapq, p->mapq, 288);
        cx = p->cx;
        cy = p->cy;
        cq = p->cq;
        step_cnt = p->step_cnt;
        done_cnt = p->done_cnt;
        done_q = p->done_q;
    }
    else
    {
        memcpy(mapq, map_q, 288);
        cx = curr_x;
        cy = curr_y;
        cq = curr_q;
        step_cnt = 0;
        done_cnt = dem_done_cnt;
        done_q = 0;
    }
    
    dx = x;
    dy = y;

    if (map_q[dx][dy] < 0)
    {
        donate_flag = 0;
    }
    else
    {
        donate_flag = 1;
    }

    done = 0;
    sim_walk();
}

int Node::sim_update()
{
    if (mapq[cx][cy] == 0)
    {
        return 0;
    }
    
    if (mapq[cx][cy] == 10000)
    {
        cq = C;
    }
    else if (mapq[cx][cy] != 0)
    {
        short sum_q = cq + mapq[cx][cy];
        if (sum_q > C)
        { 
            mapq[cx][cy] = sum_q - C;
            cq = C;
        }
        else if (sum_q < 0)
        {
            mapq[cx][cy] = sum_q;
            done_q += cq;
            cq = 0;
        }
        else
        {
            if (mapq[cx][cy] < 0) 
            {
                done_q += mapq[cx][cy];
                ++done_cnt;
            }
            mapq[cx][cy] = 0;
            cq = sum_q;
        }

        if (done_cnt == 5)
            done = 1;
    }
    return 0;
}

int Node::sim_walk()
{
    while (1)
    {
        int y_pace = (dy - cy > 0) ? 1 : -1;
        int x_pace = (dx - cx > 0) ? 1 : -1;

        if (step_cnt & 1 == 0) // 偶数步优先y
        {
            if (cy != dy) // 在y方向上移动
            {
                if (mapq[cx][cy+y_pace] < 0 && cx != dx)
                {// 如果撞到其他需求点，绕路
                    cx += x_pace;
                }
                else
                {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                    cy += y_pace;
                }
            }
            else if (cx != dx)
            {
                if (mapq[cx+x_pace][cy] < 0 && cy != dy)
                {// 如果撞到其他需求点，绕路
                    cy += y_pace;
                }
                else
                {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                    cx += x_pace;
                }
            }
            else
            {
                break;
            }
        }
        else // 奇数步优先x
        {
            if (cx != dx)
            {
                if (mapq[cx+x_pace][cy] < 0 && cy != dy)
                {// 如果撞到其他需求点，绕路
                    cy += y_pace;
                }
                else
                {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                    cx += x_pace;
                }
            }
            else if (cy != dy) // 在y方向上移动
            {
                if (mapq[cx][cy+y_pace] < 0 && cx != dx)
                {// 如果撞到其他需求点，绕路
                    cx += x_pace;
                }
                else
                {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                    cy += y_pace;
                }
            }
            else
            {
                break;
            }
        }

        sim_update();
        step_cnt += 1;
    }

    return 0;
}

void print_node(Node node)
{
    printf("<< xy(%d, %d) q(%d) step_cnt(%d) dxy(%d, %d) done_cnt(%d) >>\n", 
        node.cx, node.cy, node.cq, node.step_cnt, node.dx, node.dy, node.done_cnt);
}

int route_plan()
{
    passed_nodes.clear();
    passing_nodes.clear();
    
    Node n0(passed_nodes.end(), curr_x, curr_y);
    passing_nodes.insert(n0);

    set<Node>::iterator new_it;    
    while (1)
    {
        set<Node, cmp>::iterator it = passing_nodes.begin();
        new_it = passed_nodes.insert(*it).first;
        if (passed_nodes.size() > 300000)
            break;
        if (new_it->done == 1)
            break; // 从此节点回溯路径
        
        // 需求点
        if (new_it->cq != 0) // 身上没东西就不去需求点
        {
            for (int i = 0; i < 5; ++i)
            {
                if (new_it->mapq[dem_x[i]][dem_y[i]] == 0) continue;
                Node node(new_it, dem_x[i], dem_y[i]);
                set<Node>::iterator it_find = passed_nodes.find(node);
                if (it_find == passed_nodes.end())
                {
                    passing_nodes.insert(node);
                }
            }
        }
        
        if (new_it->cq != 100) // 身上是满的就不去捐赠点和仓库
        {
            // 捐赠点
            for (int i = 0; i < don_cnt; ++i)
            {
                if (new_it->mapq[don_x[i]][don_y[i]] == 0) continue;
                Node node(new_it, don_x[i], don_y[i]);
                set<Node>::iterator it_find = passed_nodes.find(node);
                if (it_find == passed_nodes.end())
                {
                    passing_nodes.insert(node);
                }
            }
            
            // 仓库
            Node node(new_it, orig_x, orig_y);
            set<Node>::iterator it_find = passed_nodes.find(node);
            if (it_find == passed_nodes.end())
            {
                passing_nodes.insert(node);
            }
        }        

        passing_nodes.erase(it);
    }
    
    int i = 0;
    while (new_it->parent != passed_nodes.end())
    {
        route_x[i] = new_it->dx;
        route_y[i] = new_it->dy;
        new_it = new_it->parent;
        ++i;
    }
    route_len = i;
    route_pos = i-1;
    route_step_cnt = 0;
    return 0;
}

int update()
{
    if (map_q[curr_x][curr_y] == 0)
    {
        return 0;
    }
    
    if (map_q[curr_x][curr_y] == 10000)
    {
        curr_q = C;
    }
    else
    {
        short sum_q = curr_q + map_q[curr_x][curr_y];
        if (sum_q > C)
        { 
            map_q[curr_x][curr_y] = sum_q - C;
            curr_q = C;
        }
        else if (sum_q < 0)
        {
            map_q[curr_x][curr_y] = sum_q;
            curr_q = 0;
        }
        else
        {
            if (map_q[curr_x][curr_y] < 0) 
                ++dem_done_cnt;
            map_q[curr_x][curr_y] = 0;
            curr_q = sum_q;
        }
    }
        
    if (dem_done_cnt == 5) exit(0);
    return 0;
}

int move_to(unsigned char x, unsigned char y)
{
    int y_pace = (y - curr_y > 0) ? 1 : -1;
    int x_pace = (x - curr_x > 0) ? 1 : -1;

    if (route_step_cnt & 1 == 0) // 偶数步优先y
    {
        if (curr_y != y) // 在y方向上移动
        {
            if (map_q[curr_x][curr_y+y_pace] < 0 && curr_x != x)
            {// 如果撞到其他需求点，绕路
                curr_x += x_pace;
                if (x_pace > 0) { puts("S"); fflush(stdout); }
                else { puts("N"); fflush(stdout);}
            }
            else
            {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                curr_y += y_pace;
                if (y_pace > 0) { puts("E"); fflush(stdout); }
                else { puts("W"); fflush(stdout);}
            }
        }
        else if (curr_x != x)
        {
            if (map_q[curr_x+x_pace][curr_y] < 0 && curr_y != y)
            {// 如果撞到其他需求点，绕路
                curr_y += y_pace;
                if (y_pace > 0) { puts("E"); fflush(stdout); }
                else { puts("W"); fflush(stdout);}
            }
            else
            {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                curr_x += x_pace;
                if (x_pace > 0) { puts("S"); fflush(stdout); }
                else { puts("N"); fflush(stdout);}
            }
        }
        else
        {
            puts("something went wrong...\n");
            fflush(stdout);
            exit(-1);
        }
    }
    else // 奇数步优先x
    {
        if (curr_x != x)
        {
            if (map_q[curr_x+x_pace][curr_y] < 0 && curr_y != y)
            {// 如果撞到其他需求点，绕路
                curr_y += y_pace;
                if (y_pace > 0) { puts("E"); fflush(stdout); }
                else { puts("W"); fflush(stdout);}
            }
            else
            {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                curr_x += x_pace;
                if (x_pace > 0) { puts("S"); fflush(stdout); }
                else { puts("N"); fflush(stdout);}
            }
        }
        else if (curr_y != y) // 在y方向上移动
        {
            if (map_q[curr_x][curr_y+y_pace] < 0 && curr_x != x)
            {// 如果撞到其他需求点，绕路
                curr_x += x_pace;
                if (x_pace > 0) { puts("S"); fflush(stdout); }
                else { puts("N"); fflush(stdout);}
            }
            else
            {// 如果不会撞到其他需求点或另一个方向已经到达终线，正常走
                curr_y += y_pace;
                if (y_pace > 0) { puts("E"); fflush(stdout); }
                else { puts("W"); fflush(stdout);}
            }
        }
        else
        {
            puts("something went wrong...\n");
            fflush(stdout);
            exit(-1);
        }
    }

    route_step_cnt += 1;
    
    return 0;
}

int new_node(unsigned char x, unsigned char y, short num)
{
    if (kk < 5)
    {
        dem_x[kk] = x;
        dem_y[kk] = y;
        total_dem_q -= num;
    }
    else
    {
        int j = kk - 5;
        don_x[j] = x;
        don_y[j] = y;
        ++don_cnt;
    }
    map_q[x][y] = num;
    ++kk;
    
    return 0;
}

int main_loop()
{
    char s[50];
    // unsigned char x, y;
    int x, y;
    short num;

    for (;;)
    {
        gets(s);
        switch (s[0])
        {
        case 'S':
        case 's':
            // 仓库位置
            // sscanf(s+1, " %hhu %hhu", &x, &y);
            sscanf(s+1, " %d %d", &x, &y);
            orig_x = x;
            orig_y = y;
            map_q[x][y] = 10000;
            curr_x = orig_x;
            curr_y = orig_y;
            break;
        case 'R':
        case 'r':
            // 需求点或捐赠点
            // sscanf(s+1, " %hhu %hhu %hd", &x, &y, &num);
            sscanf(s+1, " %d %d %hd", &x, &y, &num);
            new_node(x, y, num);
            update();
            // 清空路径
            route_len = 0;
            break;
        case 'G':
        case 'g':
            // 如果没有路径，规划之，否则略过
            if (route_len == 0) route_plan();
            // 按照路径走
            move_to(route_x[route_pos], route_y[route_pos]);
            update();
            if (curr_x == route_x[route_pos] && curr_y == route_y[route_pos])
            {
                --route_pos;
                if (route_pos < 0) route_len = 0;
                route_step_cnt = 0;
            }
            // disp_qmap();
            break;
        default:
            break;
        }
    }

    return 0;
}

int main()
{
    main_loop();
    return 0;
}
