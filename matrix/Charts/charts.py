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
    tp = np.zeros((8,), dtype=float);

    sp = np.zeros((8,), dtype=float);

    sum_info = open('merge-sort.txt', 'r')
    sum_info = sum_info.read().split('\n')
    for i in range(0,9):
        sum_info.remove("");

    for i in range(0,10):
        ts += float(sum_info[i])

    for i in range(1,9):
        for j in range(0,10):
            tp[i-1] += float(sum_info[10*i + j])
        tp[i-1] /= 10

    ts /= 10.0

    for i in range(0, 8):
        sp[i] = ts / tp[i]

    return sp

def plot_chart(speedup):
    ideal = []

    speedup = np.insert(speedup, 0, 0, axis=0)
    print(speedup)


    for i in range(0,9):
        ideal.insert(i, i)

    plt.ylim(1, 8)
    plt.xlim(1, 8)

    plt.plot(ideal, color='black')
    plt.plot(speedup, color='blue')
    plt.xlabel('Numero de cores')
    plt.ylabel('Speedup')
    plt.title('Speedup da Ordenação')
    plt.show()

def main():
    speedup = setup()
    plot_chart(speedup)

if __name__ == '__main__': main()
