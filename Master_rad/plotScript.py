import pandas as pd
import matplotlib.pyplot as plt

import numpy as np

def plot(algorithm):

    df = pd.read_csv("benchDense.csv", sep=';')
    
    df = df[df['algo'] == algorithm]
    print(df)
    plt.figure(figsize=(7, 5), dpi=100)
    plt.plot(df["n"], df["ms"], marker=",")
    
    plt.xlabel("Broj čvorova")
    plt.ylabel("Vreme izvršavanja(ms)")
    plt.title("Vreme izvršavanja u zavisnosti od veličine ulaza (broja čvorova)")
    plt.grid(False)
    plt.tight_layout()
    
    plt.savefig(algorithm + "3.png", dpi=150)
    plt.show()


import pandas as pd
import matplotlib.pyplot as plt

import numpy as np

def plotAll():

    df = pd.read_csv("benchDense.csv", sep=';')

    plt.figure(figsize=(14, 8), dpi=100)
    
    prim = df[df['algo'] == 'Prim']
    plt.plot(prim["n"], prim["ms"], marker=",", color='r', label="Primov algoritam")
    #print(df)
    kruskal = df[df['algo'] == 'Kruskal']
    plt.plot(kruskal["n"], kruskal["ms"], marker=",", color='g', label="Kruskalov algoritam")

    boruvka = df[df['algo'] == 'Boruvka']
    plt.plot(boruvka["n"], boruvka["ms"], marker=",", color='b', label="Boruvkin algoritam")

   # reverseDelete = df[df['algo'] == 'RevereseDelete']
   # plt.plot(reverseDelete["n"], reverseDelete["ms"], marker=",", color='c')

    karger = df[df['algo'] == 'Karger']
    plt.plot(karger["n"], karger["ms"], marker=",", color='m', label="Kargerov algoritam")

    fredmanTarjan = df[df['algo'] == 'FredmanTarjan']
    plt.plot(fredmanTarjan["n"], fredmanTarjan["ms"], marker=",", color='y', label="Fredman-Tardžanov algoritam")

    chazelle = df[df['algo'] == 'Chazelle']
    plt.plot(chazelle["n"], chazelle["ms"], marker=",", color='k', label="Šazelov algoritam")
    
    plt.xlabel("Broj čvorova")
    plt.ylabel("Vreme izvršavanja(ms)")
    plt.title("Vreme izvršavanja u zavisnosti od veličine ulaza (broja čvorova)")
    plt.grid(False)
    plt.legend(title="", loc="best")
    plt.tight_layout()
    
    plt.savefig("all2.png", dpi=150)
    plt.show()