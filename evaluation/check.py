# -*- coding: utf-8 -*-
# 对排名结果进行测试
# 目前支持的方法有：AUC, Precision，Time_distribution


import os
import sys
import math
import codecs
import numpy as np
from distutils.log import warn as printf
from operator import itemgetter


def precision(data_list, reward_list, scope=200, slice=10):
    '''
    计算precision
    :param data_list:
    :param reward_list:
    :param scope:
    :param slice:
    :return:
    '''
    sum = 0
    combine_list = []
    result_list = []
    reward_set = set([])
    for x in reward_list:
        reward_set.add(int(x))
    for item in data_list:
        if item[0] in reward_set:
            combine_list.append((1, item[1]))
        else:
            combine_list.append((0, item[1]))
    combine_list.sort(key = lambda item: item[1],reverse=True)

    for i in range(min(scope, len(combine_list))):
        sum += combine_list[i][0]
        if (i + 1) % slice == 0:
            result_list.append(sum * 1.0 / (i + 1))
        else:
            pass
    printf('precision: ' + str(result_list))

def recall(data_list, reward_list,L=628):
    count = 1
    total_num = 0
    reward_set = set(reward_list)
    for item in data_list:
        if count <= L:
            if item[0] in reward_set:
                total_num += 1
            count += 1
        else:
            break
    recall = total_num/len(reward_set)
    printf('recall: '+ str(recall))
    # sum = 0
    # combine_list = []
    # result_list = []
    # reward_set = set([])
    # for x in reward_list:
        # reward_set.add(int(x))
    # for item in data_list:
        # if item[0] in reward_set:
            # combine_list.append((1, item[1]))
        # else:
            # combine_list.append((0, item[1]))
    # #combine_list.sort(key = lambda item: item[1],reverse=True)
    # #combine_list = sorted(combine_list,key = itemgetter(1),reverse=True)

    # #print(combine_list[:100])
    # for i in range(min(scope, len(combine_list))):
        # sum += combine_list[i][0]
        # if (i + 1) % slice == 0:
            # result_list.append(sum * 1.0 / (len(reward_set)))
        # else:
            # pass
    # printf('recall: ' + str(result_list))


def recall_year(data_list, reward_list, L=628):
    reward_dict = {}
    topL_set = set()
    count = 1
    year_to_num = {}
    each_year = {}
    for item in reward_list:
        if int(item[0]) not in reward_dict:
            reward_dict[int(item[0])] = int(item[1])
            if item[1] in each_year.keys():
                each_year[int(item[1])] += 1
            else:
                each_year[int(item[1])] = 1
    #print(reward_dict)
    count = 1
    for item in data_list:
        if count <= L:
            topL_set.add(int(item[0]))
            count += 1
    for id in reward_dict:
        if id in topL_set:
            #print(int(item))
            if reward_dict[id] in year_to_num:
                year_to_num[reward_dict[id]] += 1
            else:
                year_to_num[reward_dict[id]] = 1

    for year in year_to_num:
        year_to_num[year] = year_to_num[year]/each_year[year]#每一年的recall值等于top-L中该年获奖数/reward_list中该年的获奖数
    sort_year_to_num = sorted(year_to_num.items(),key = itemgetter(0))
    printf("all year's recall is:")
    printf(sort_year_to_num)

def NDCG(data_list, reward_list, k=50):
    sum = []
    count = 1
    final_result = 0
    #计算前k=50的排名列表的NDCG
    reward_set = set([])
    combine_list = []
    for x in reward_list:
        reward_set.add(int(x))
    for item in data_list:
        if count <= k:
            if item[0] in reward_set:
                sum.append(1/(np.log2(count+1)))
            else:
                sum.append(0)
            count += 1
    max_num = max(sum)
    min_num = min(sum)
    if max_num != 0:
        for i in sum:
            final_result += (i-min_num)/(max_num-min_num)
    else:
        final_result = 0
    print('NDCG_'+str(k)+':'+str(final_result))
    

def precision_year(data_list, reward_list, L=628):
    reward_dict = {}
    topL_set = set()
    count = 1
    year_to_num = {}
    each_year = {}
    for item in reward_list:
        if int(item[0]) not in reward_dict:
            reward_dict[int(item[0])] = int(item[1])
            if item[1] in each_year.keys():
                each_year[int(item[1])] += 1
            else:
                each_year[int(item[1])] = 1
    count = 1
    for item in data_list:
        if count <= L:
            topL_set.add(int(item[0]))
            count += 1
    for id in reward_dict:
        if id in topL_set:
            if reward_dict[id] in year_to_num:
                year_to_num[reward_dict[id]] += 1
            else:
                year_to_num[reward_dict[id]] = 1

    for year in year_to_num:
        year_to_num[year] = year_to_num[year]/L
    sort_year_to_num = sorted(year_to_num.items(),key = itemgetter(0))
    printf("all year's precision is:")
    printf(sort_year_to_num)

def auc(data_list, reward_list):
    '''
    计算auc值，reward_file记得转换成Int型
    :param data_list:
    :param reward_list:
    :return:
    '''
    ree = []
    nl = []
    reN = 0
    nlN = 0
    k = 0
    reward_set = set([])
    for x in reward_list:
        reward_set.add(int(x))

    for item in data_list:
        if item[0] in reward_set:
            ree.append(item)
            reN += 1
        elif item[1] != 0:  # 修正
            nl.append(item)
            nlN += 1
        else:
            pass

    for x in ree:
        for y in nl:
            if (x[1] > y[1]):
                k += 1
            elif (x[1] == y[1]):
                k += 0.5
            else:
                pass
    printf("AUC" + '(' + str(reN) + ':' + str(nlN) + '): ' + str(k * 1.0 / (reN * nlN)))


def read_column(file_path, column=0):
    target_list = []
    with codecs.open(file_path, 'r', 'utf8') as fp:
        for x in fp.readlines():
            item = float(x.split('\t')[column])  # float
            target_list.append(item)
    return target_list


def time_distribution(data_list, time_list, limit=0.01, slices=30):
    '''
    # 使用的时候有问题
    :param data_list: 数据列表
    :param time_list: 时间列表
    :param limit: 比例
    :param slices: 切成多少份
    :return:
    '''
    combine_list = []
    distribution_list = []
    combine_dict = dict()
    slice_length = (len(time_list) // slices)
    for i in range(slices):
        distribution_list.append(0)

    counter = 0
    for item in data_list:
        #if item[1] != -11111111:
        if item[1] != 0:
            node = [item[0], item[1], 0]
            combine_list.append(node)
            combine_dict[str(int(item[0]))] = counter
            counter += 1
    data_length = len(combine_list)
    #assert data_length == len(time_list)
    printf("data_length:" + str(data_length))

    # 添加时间排序信息
    flag = -1
    for j in range(len(time_list)):
        if j % slice_length == 0 and flag < (slices - 1):
            flag += 1
        combine_list[combine_dict[str(int(time_list[j]))]][2] = flag
    combine_list.sort(key = lambda item: item[1],reverse=True)

    for z in range(int(data_length * limit)):
        distribution_list[combine_list[z][2]] += 1
    printf(distribution_list)

    # 计算Balance值
    def rstd(data):#实际标准差
        return np.std(data)

    def estd(N, S, z):#理想标准差
        return math.sqrt(z * N / S * (1 - 1 / S) * (1 - z) * N / (N - 1))

    printf("balance: " +        str((rstd(distribution_list) / estd(data_length, slices, limit)) - 1))

def time_distribution_new(data_list, time_list, limit=0.01, slices=30, top=267):
    max_year = 2015
    min_year = 1995
    slices = max_year - min_year + 1
    slice_length = (len(time_list) // (slices))
    data_length = len(time_list)
    distribution_dict = {}
    count = 0
    for item in data_list:
        if count <= top:
            if item[3] in distribution_dict:
                distribution_dict[item[3]] += 1
            else:
                distribution_dict[item[3]] = 1
            count += 1
    distribution_list = list(distribution_dict.values())
    printf(distribution_dict)
    def rstd(data):#实际标准差
        return np.std(data)
    def estd(N, S, z):#理想标准差
        return math.sqrt(z * N / S * (1 - 1 / S) * (1 - z) * N / (N - 1))
    #print(estd(data_length, slices, limit))
    #print(rstd(distribution_list))
    printf("balance: " +        str(((rstd(distribution_list) / estd(data_length, slices, limit)) - 1)))

def exact_group_score(data_list, time_list, slices=40, group=1, input_path=''):
    slice_length = (len(time_list)//(slices))+1
    group_item = {}
    if group != 40:
        for item in range(slice_length*(group-1),slice_length*group):
            group_item[int(time_list[item])] = 0
    else:
        for item in range(slice_length*(group-1),len(time_list)):
            group_item[int(time_list[item])] = 0
    for item in data_list:
        if item[0] in group_item:
            group_item[item[0]] = item[1]
    (input_dir, temp_file_name)= os.path.split(input_path)
    (shotname,extension) = os.path.splitext(temp_file_name)
    file_name = shotname + '_Group' + str(group)+extension
    result_out = open(file_name,'w')
    for (key,value) in group_item.items():
        result_out.write('%s\t%s\n' %(key,value))
    result_out.close()
    return group_item


if __name__ == '__main__':
    #dir_a = 'amazon_music'  # 排名结果所在目录
    dir_a = './wj_netflix_com_result/new20u&i/BIRANK_time'
    dir_b = './evaluation_data'  # 评价集所在目录
    #files_b = ['netflix_com.auc']  # 评价文件#awarded list
    files_b = ['netflix_com_20u&i.sortbytime']

    for rt, dirs, files in os.walk(dir_a):
        for a in files:
            file_a_path = os.path.join(dir_a, a)
            #list_a = zip(read_column(file_a_path, 0), read_column(file_a_path, 1))  # 手动设置项_无年份
            list_a = zip(read_column(file_a_path, 0), read_column(file_a_path, 1),read_column(file_a_path, 2),read_column(file_a_path,3))#有时间+年份
            # list_a = read_column(file_a_path, 1)
            for b in files_b:
                file_b_path = os.path.join(dir_b, b)
                list_b = read_column(file_b_path, 0)  # 手动设置项
                #list_c = zip(read_column(file_b_path,0), read_column(file_b_path, 2))#年份计算
                #printf(a + ' : ' + str(len(list_a)) + ' | ' + b + ' : ' + str(len(list_b)))
                printf(a + ' | ' + b)
                #recall(list_a, list_b,177)
                #print("ok recall")
                #recall_year(list_a,list_c,177)
                #precision_year(list_a,list_c,628)
                #exact_group_score(list_a,list_b,40,40,file_a_path)
                #NDCG(list_a, list_b,50)
                #auc(list_a, list_b)
                #precision(list_a, list_b, 177,177)
                time_distribution(list_a, list_b, limit=0.01, slices=40)
                #time_distribution_new(list_a, list_b, limit=0.01, slices=40, top=177)
                # print pearsonr(list_a, list_b)
                # print kendalltau(list_a, list_b)
                # echoRank(list_a, list_b)
