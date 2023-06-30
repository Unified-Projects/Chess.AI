ValidatedFens = []

with open("../build/Fens.txt") as f:
    data = f.read().split("\n\n")

    for x in data:
        if(len(x) < 10):
           continue

        xdata = x.split("\n")

        ValidatedFens.append(xdata[0])

FenComparative = ""

with open("fens.txt") as f:
    FenComparative = f.read()

for x in ValidatedFens:
    if not FenComparative.find(x):
        print(x)