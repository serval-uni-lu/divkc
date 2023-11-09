import random
import argparse

import PartitionInfo
import dDNNF

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

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--cnf", type=str, help="path to the cnf formula")
# parser.add_argument("-k", type=int, default=50)
parser.add_argument("-n", type=int, default=100, help="number of samples")

args = parser.parse_args()
cnf_file = args.cnf

res = PartitionInfo.from_file(cnf_file + ".log")

nb_vars = 0
for i in res.part_sizes:
    nb_vars += res.part_sizes[i]

emc = 1
res.nnf = dict()
for i in res.part:
    pnnf = cnf_file + "." + res.files[i] + ".nnf"
    nnf = dDNNF.from_file(pnnf)
    nnf.annotate_mc()

    res.nnf[i] = nnf

    lmc = nnf.get_node(1).mc / 2**(nb_vars - res.part_sizes[i])
    emc *= lmc

print(emc)
