import numpy as np
import subprocess
import shlex
import time

SEED_BASE = 1000 # 指定seed，保证每次运行生成的1000张图是一样的，便于调优
MAP_CNT = 1000

mask_program = '<你的配送程序>'

cmd = shlex.split(mask_program)

step_counts = []
total_time = time.time()
failure_cnt = 0

C = 100


for i in range(MAP_CNT):
    np.random.seed(i*SEED_BASE)
    history = ''
    ending = ''
    nodes = []
    step_cnt = 0

    curr_x = 0
    curr_y = 0
    curr_q = C
    qmap = np.zeros((12,12), dtype=int)
    demand = []

    begin_time = time.time()

    # 开始
    p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=1, universal_newlines=True)

    # 生成仓库
    xo = np.random.randint(0, 12, size=1)[0]
    yo = np.random.randint(0, 12, size=1)[0]
    nodes.append((xo, yo))
    curr_x = xo
    curr_y = yo
    qmap[xo,yo] = 10000
    p.stdin.write('S %d %d\n' %(xo, yo))
    p.stdin.flush()
    # print('S %d %d\n' %(xo, yo))
    history += 'S %d %d\n' %(xo, yo)

    # 生成需求点
    j = 0
    while j < 5:
        xj = np.random.randint(0, 12, size=1)[0]
        yj = np.random.randint(0, 12, size=1)[0]
        node = (xj, yj)
        if node in nodes:
            continue
        else:
            j += 1
        nodes.append(node)
        demand.append(node)
        num = -np.random.randint(50, 201, size=1)[0]
        qmap[xj, yj] = num
        p.stdin.write('R %d %d %d\n' %(xj, yj, num))
        p.stdin.flush()
        # print('R %d %d %d\n' % (xj, yj, num))
        history += 'R %d %d %d\n' % (xj, yj, num)

    # 运输 + 生成捐赠点
    don_gen_step = np.random.randint(10, size=1) + 11
    while True:
        if don_gen_step == 0 :
            don_gen_step = np.random.randint(10, size=1) + 11
            if len(nodes) == 144:
                continue
            while True:
                xj = np.random.randint(0, 12, size=1)[0]
                yj = np.random.randint(0, 12, size=1)[0]
                node = (xj, yj)
                if node in nodes:
                    continue
                else:
                    nodes.append(node)
                    num = np.random.randint(1, 101, size=1)[0]
                    qmap[xj, yj] = num
                    p.stdin.write('R %d %d %d\n' %(xj, yj, num))
                    p.stdin.flush()
                    # print('R %d %d %d\n' % (xj, yj, num))
                    history += 'R %d %d %d\n' %(xj, yj, num)
                    break
        else:
            p.stdin.write('G\n')
            p.stdin.flush()
            history += 'G\n'
            c = p.stdout.readline()
            if len(c) != 0:
                if c[0] in ['N','S','W','E']:
                    q = curr_q + qmap[curr_x, curr_y]
                    if q > C:
                        qmap[curr_x, curr_y] = q - C
                        curr_q = C
                    elif q < 0:
                        qmap[curr_x, curr_y] = q
                        curr_q = 0
                    else:
                        curr_q = q
                        qmap[curr_x, curr_y] = 0

                    if c[0] == 'N': curr_x -= 1
                    elif c[0] == 'S': curr_x += 1
                    elif c[0] == 'W': curr_y -= 1
                    elif c[0] == 'E': curr_y += 1
                    else:
                        print('wrong!')
                    q = curr_q + qmap[curr_x, curr_y]
                    if q > C:
                        qmap[curr_x, curr_y] = q - C
                        curr_q = C
                    elif q < 0:
                        qmap[curr_x, curr_y] = q
                        curr_q = 0
                    else:
                        curr_q = q
                        qmap[curr_x, curr_y] = 0
                else:
                    # incorrect
                    ending = ' 配送中断'
                    failure_cnt += 1
                step_cnt += 1
                don_gen_step -= 1
            else:
                allright = 1
                for d in demand:
                    if qmap[d] != 0:
                        allright = 0
                if allright == 0:
                    ending = ' 有小区未配送'
                    failure_cnt += 1
                else:
                    ending = ' 配送完成'
                step_counts.append(step_cnt)
                break
    
    end_time = time.time()
    t = round((end_time - begin_time) * 1000, 3)
    print(f'第 {i+1} 张地图，花费步数：{step_cnt}，耗时:{t}ms' + ending)
    time.sleep(0.01)

avg = np.array(step_counts).mean()
total_time = time.time() - total_time
total_time = round(total_time, 2)
print(f'平均花费步数：{avg}，总耗时：{total_time}s，失败：{failure_cnt}')