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



    sum_info = open('sum.txt', 'r')
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

    # fig, ax = plt.subplots(2,1)
    # fig, ax = plt.subplots(3,1)

    # ax[0].plot(speedup, color='blue')
    # ax[0].grid(True)
    # plt.set_xlabel('Iteração')
    # plt.set_ylabel('Cláusulas')
    # plt.set_title('Speedup')

    # ax[0].plot(rand_result, color='yellow')
    # ax[0].set_xlabel('Iteração')
    # ax[0].set_ylabel('Cláusulas')
    # ax[0].set_title('Busca Aleatória')


    # ax[1].plot(temperature, color='red')
    # ax[1].yaxis.grid(True)
    # ax[1].set_xlabel('Iteração')
    # ax[1].set_ylabel('Temperatura')
    # ax[1].set_title('Temperatura')

    # plt.tight_layout()
    plt.show()

    # fig.savefig("chart.p'ng")

def main():


    speedup = setup()
    plot_chart(speedup)

    # for it in range(1, 10):
    #     f, t, r= setup(filepath, file_, schedule, it)
    #
    #     for j in range(0, len(fitness)):
    #         fitness[j] += f[j]
    #
    #     for j in range(0, len(temperature)):
    #         temperature[j] += t[j]
    #
    #     for j in range(0, len(rand_result)):
    #         rand_result[j] += r[j]
    #
    # for it in range(0, len(fitness)):
    #     fitness[it] /= 10.0
    #
    # for it in range(0, len(temperature)):
    #     temperature[it] /= 10.0
    #
    # for it in range(0, len(rand_result)):
    #     rand_result[it] /= 10.0
    #
    # plot_chart_SA(fitness, temperature, rand_result, file_, schedule)
    # plot_chart_RS(fitness, temperature, rand_result, file_, schedule)
    # plot_mix_chart(fitness, temperature, rand_result, file_, schedule)


if __name__ == '__main__': main()
