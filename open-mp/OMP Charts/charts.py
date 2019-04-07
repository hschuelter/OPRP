#-*- coding:utf-8 -*-
import os
import math
import random
import argparse
import numpy as np
import matplotlib.pyplot as plt

# === SETTING UP === #
def setup():
    ts = 0
    tp = np.zeros((8,), dtype=float)
    tp_omp = np.zeros((9,), dtype=float)

    sp = np.zeros((8,), dtype=float)
    sp_omp = np.zeros((9,), dtype=float)

    sum_info = open('sum-1.txt', 'r')
    sum_info = sum_info.read().split('\n')

    omp_sum =  open('sum-2.txt', 'r')
    omp_sum = omp_sum.read().split('\n')

    for i in range(0,9):
        sum_info.remove("")
        omp_sum.remove("")
    omp_sum.remove("")
    
    for i in range(0,10):
        ts += float(sum_info[i])

    for i in range(1,9):
        print(i)
        for j in range(0,10):
            tp[i-1] += float(sum_info[10*i + j])
        tp[i-1] /= 10
        
    for i in range(0,9):
        for j in range(0,9):
            tp_omp[i] += float(omp_sum[10*i + j])
        tp_omp[i] /= 10

    ts /= 10.0

    for i in range(0, 8):
        sp[i] = ts / tp[i]

    for i in range(1, 9):
        sp_omp[i] = ts / tp_omp[i]

    return sp, sp_omp

def plot_chart(speedup, speed_omp):
    ideal = []

    speedup = np.insert(speedup, 0, 0, axis=0)
    print(speedup)
    print(speed_omp)


    for i in range(0,9):
        ideal.insert(i, i)

    plt.ylim(1, 8)
    plt.xlim(1, 8)

    plt.plot(ideal, color='black')
    plt.plot(speedup, color='blue')
    plt.plot(speed_omp, color='green')
    plt.grid(axis='both')
    plt.xlabel('Número de threads')
    plt.ylabel('Speedup')
    plt.suptitle('Speedup da Adição', fontsize = 16)
    plt.title('Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz', fontsize = 10, loc='left')
    plt.show()

def main():
    speedup, speed_omp = setup()
    plot_chart(speedup, speed_omp)

if __name__ == '__main__': main()
