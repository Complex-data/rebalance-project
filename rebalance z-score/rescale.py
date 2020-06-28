# -*- coding: utf-8 -*-

import os
import sys
import math
import codecs
import numpy as np
from distutils.log import warn as printf


def rescaled_data(input_path, cal, delta_p, limit, slices):
    printf('starting rescale data')
    data_list = []
    p_list = []
    score_list = []
    distribution_list = []

    with codecs.open(input_path, mode='r', encoding='utf8') as infile:
        for line in infile.readlines():
            node = line.split()
            # assert len(node) == 3
            node = [int(node[0]), float(node[1]), int(node[2]), int(node[3]), 0, 0]
            if node[1] != 0:
                data_list.append(node)

    # 核对有效数据的长度
    data_length = len(data_list)
    printf('data_length: '+str(data_length))

    data_list.sort(key = lambda item: item[2])#将items按时间进行排序



    slice_length = (data_length // slices)
    for i in range(slices):
        distribution_list.append(0)

    flag = -1
    for i in range(data_length):
        if i < delta_p//2:
            p_list = data_list[0:delta_p]
        elif i >= data_length - delta_p//2:
            p_list = data_list[data_length - delta_p//2:data_length]
        else:
            p_list = data_list[i-delta_p//2:i+delta_p//2]

        score_list = []
        for node in p_list:
            score_list.append(node[1])

        avg = np.mean(score_list)
        std = np.std(score_list)
        if cal:
            data_list[i][4] = (data_list[i][1] - avg)/std
        else:
            data_list[i][4] = data_list[i][1]

        if i % slice_length == 0 and flag < (slices-1):
            flag += 1
        data_list[i][5] = flag

    # 开始计算time_distribution
    data_list.sort(key = lambda item: item[4], reverse=True)
    for z in range(limit):
        distribution_list[data_list[z][5]] += 1
    printf(distribution_list)

    if cal:
        (input_dir, temp_file_name)= os.path.split(input_path)
        (shotname,extension) = os.path.splitext(temp_file_name);
        file_name = shotname + '_' + str(delta_p) + '_rescaled'+extension
        output_path = os.path.join(input_dir, file_name)
        with codecs.open(output_path, 'w', 'utf8') as ofile:
            for node in data_list:
                ofile.write(str(node[0])+'\t'+str(node[4])+'\t'+str(node[2])+'\t'+str(node[3])+'\n')


if __name__ == '__main__':
    #input_dir = 'amazon_music'
    input_dir = './wj_netflix_com_result/new20u&i/BIRANK_time'
    #input_file = 'f10_items_0_0_1_0_1_0_0'
    input_file = 'birank_time_withtime.txt'
    input_path = os.path.join(input_dir, input_file)
    #for i in range(50,2050,50):
    rescaled_data(input_path, cal=True, delta_p=1000, limit=410, slices=40)
