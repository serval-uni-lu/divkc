import random
import argparse

import pycosat

import PartitionInfo
import dDNNF
import DIMACS

def sample(nnf):
    stack = [nnf.get_node(1)]
    sample = []

    while len(stack) > 0:
        # cid = stack.pop()
        node = stack.pop()

        if type(node) is dDNNF.OrNode:
            key = random.randint(1, node.mc)

            for c in node.children:
                tmc = c.target.mc * (2**len(c.free))

                if key <= tmc:
                    for i in c.free:
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
                sample.append(random.choice([i, -i]))

            for i in node.child.consts:
                sample.append(i)

        elif type(node) is dDNNF.TrueNode:
            pass
        elif type(node) is dDNNF.FalseNode:
            raise ValueError('encountered FalseNode wihle sampling')

    return sample

def project(sample, vset):
    return [x for x in sample if abs(x) in vset]

def project_nnf(nnf, vset):
    for i in nnf.ordering:
        node = nnf.get_node(i)

        if type(node) is dDNNF.OrNode:
            for c in node.children:
                c.free = [x for x in c.free if x in vset]
                c.consts = [x for x in c.consts if abs(x) in vset]
        elif type(node) is dDNNF.UnaryNode:
            node.child.free = [x for x in node.child.free if x in vset]
            node.child.consts = [x for x in node.child.consts if abs(x) in vset]

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--cnf", type=str, help="path to the cnf formula")
# parser.add_argument("-k", type=int, default=50)
parser.add_argument("-n", type=int, default=100, help="number of samples")

args = parser.parse_args()
cnf_file = args.cnf

res = PartitionInfo.from_file(cnf_file + ".log")

dimacs = DIMACS.from_file(cnf_file)

nb_vars = 0
for i in res.part_sizes:
    nb_vars += res.part_sizes[i]

emc = 1

res.nnf = dict()
for i in res.part:
    pnnf = cnf_file + "." + res.files[i] + ".nnf"
    nnf = dDNNF.from_file(pnnf)
    project_nnf(nnf, res.part[i])
    nnf.annotate_mc()

    res.nnf[i] = nnf

    # lmc = nnf.get_node(1).mc / 2**(nb_vars - res.part_sizes[i])
    lmc = nnf.get_node(1).mc
    emc *= lmc

print(emc)

nb_samples = 0
nb_tries = 0

while nb_samples < args.n:
    nb_tries += 1

    cs = []
    for i in res.nnf:
        tmp = sample(res.nnf[i])
        # tmp = project(tmp, res.part[i])
        cs += tmp

    tcnf = dimacs.cls + [[x] for x in cs]
    pres = pycosat.solve(tcnf)

    if pres != "UNSAT":
        cs.sort(key=abs)
        print(' '.join(map(str, cs)))
        nb_samples += 1

print(nb_samples / nb_tries)
print(emc * nb_samples / nb_tries)
