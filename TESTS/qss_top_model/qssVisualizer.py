
steps = 100
delta_t = 1
fileName = 'inputs/ANALOG_In.txt'

import numpy as np
import matplotlib.pyplot as plt
import math
import random
import matplotlib.patches as mpatches

def inputGenerator(x):

    # if x <= 100:
        # return math.sin(x)
    # if x <= 200:
        # return math.sin(100) - math.cos(x)
    # else:
        # return math.sin(100) - math.cos(200) + math.log(x)
    
    #return (3*x*x*x)/(200*200*200) - (x*x)/(100*100)
    return (x*x)/(100.0*100.0)
    #return math.sin(6.29*x/100)


def writeToInputFile():
    in_list = []

    for i in range(steps):
        in_list.append(inputGenerator(delta_t*i))
        

    for index in range(steps):
        in_list[index] = in_list[index]*(1 + random.randint(0,1)/50.0)

    str = ''
    for i in range(len(in_list)):
        if i == (len(in_list) - 1):
            str = str + '00:00:00:{} {}'.format(i,in_list[i])
        else:
            str = str + '00:00:00:{} {}\n'.format(i,in_list[i])
        
    f = open(fileName, 'w')
    f.write(str)
    f.close()


def plotInputOutput():
    f = open('outputs/qssReceiver_Out.txt', 'r')
    str = f.read()
    f.close()
    lines = str.split('\n')

    out_list = []
    for line in lines:
        if line == '':
            break
        value = line.split(' ')[1]
        out_list.append(float(value))
        
        
    f = open('inputs/ANALOG_In.txt', 'r')
    str = f.read()
    f.close()
    lines = str.split('\n')

    in_list = []
    for line in lines:
        if line == '':
            break
        value = line.split(' ')[1]
        in_list.append(float(value))


    plt.plot(in_list,'bo',markersize=2)
    plt.plot(out_list,'ro',markersize=2)
    plt.xlabel('sampling steps')
    plt.ylabel('y (signal value)')

    sampled = mpatches.Patch(color='blue',label='Sampled Points')
    plt.legend(handles=[sampled])
    reconstructed = mpatches.Patch(color='red',label='Reconstructed Points')
    plt.legend(handles=[sampled,reconstructed])
    plt.title("QSS input signal VS  QSS reconstructed signal")
    plt.show()




print("Generate input points and write to ANALOG_In.txt, or visualize output after run testing\n")
a = input("Enter 1 for Write to input file, 2 for visualzing\n")
a = int(a)
if (a == 1):
    writeToInputFile()
if (a == 2):
    plotInputOutput()