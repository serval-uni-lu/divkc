import random
import argparse
import statistics
import math

import pycosat
import scipy.stats as st

import dDNNF
import DIMACS

def sample(nnf, assumps):
    stack = [nnf.get_node(1)]
    sample = []

    while len(stack) > 0:
        # cid = stack.pop()
        node = stack.pop()

        if type(node) is dDNNF.OrNode:
            key = random.randint(1, node.mc)

            for c in node.children:
                # tmc = c.target.mc * (2**len(c.free))
                tmc = c.mc

                if key <= tmc and tmc != 0:
                    for i in c.free:
                        if i not in assumps and -1 * i not in assumps:
                            sample.append(random.choice([i, -i]))
                    for i in c.consts:
                        sample.append(i)

                    stack.append(c.target)
                    break
                else:
                    key -= tmc

        elif type(node) is dDNNF.AndNode:
            for c in node.children:
                if c.target.mc > 0:
                    stack.append(c.target)

        elif type(node) is dDNNF.UnaryNode:
            stack.append(node.child.target)

            for i in node.child.free:
                if i not in assumps and -1 * i not in assumps:
                    sample.append(random.choice([i, -i]))

            for i in node.child.consts:
                sample.append(i)

        elif type(node) is dDNNF.TrueNode:
            pass
        elif type(node) is dDNNF.FalseNode:
            raise ValueError('encountered FalseNode wihle sampling')

    return sample

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--cnf", type=str, help="path to the cnf formula")
# parser.add_argument("-k", type=int, default=50)
parser.add_argument("-n", type=int, default=100, help="number of samples")

args = parser.parse_args()
cnf_file = args.cnf

dimacs = DIMACS.from_file(cnf_file)
nnf = dDNNF.from_file(cnf_file + ".nnf")

nnf.annotate_mc()
mc = nnf.get_node(1).mc

nbv = dimacs.nb_vars
nbc = len(dimacs.cls)
varl = list(range(1, nbv + 1))

print(f'c mc {mc}')
print(f'c nv {nbv}')
print(f'c nc {nbc}')

print("--------------------")

za = st.norm.isf(0.1 / 2)
vs = random.sample(varl, 10)

def approx():
    emcl = []

    for i in range(0, 1000):
        assumps = set([random.choice([x, -x]) for x in vs])
        nnf.annotate_mc(assumps)
        emcl.append(nnf.get_node(1).mc)

    nb = len(emcl)
    amc = statistics.mean(emcl)
    s2 = sum([(x - amc)**2 for x in emcl]) / (len(emcl) - 1)
    s = math.sqrt(s2 / len(emcl))

    low = amc - za * s
    high = amc + za * s

    c = 2**len(vs)
    print(f'c emc {amc * c}')
    print(f'c lmc {low * c}')
    print(f'c high {high * c}')

    return (amc * c, low * c, high * c)

nb_t = 0
nb_ok = 0
for i in range(0, 1000):
    e, l, h = approx()
    nb_t += 1

    if l <= mc and mc <= h:
        nb_ok += 1

print(f"c t {nb_t}")
print(f"c ok {nb_ok}")
print(f"c r {nb_ok / nb_t}")


