import argparse

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

args = parser.parse_args()
cnf_file = args.cnf

dimacs = DIMACS.from_file(cnf_file)
nb_vars = dimacs.nb_vars

res = dDNNF.from_file(cnf_file + ".up.nnf")
res.annotate_mc()

tmp = ", ".join([str(i) for i in range(1, nb_vars + 1)])
print("file, f, mc, " + tmp)

print(f"{cnf_file}, up, {res.get_node(1).mc}", end = '')

for v in range(1, nb_vars + 1):
    tmc = res.get_node(1).mc
    if v in res.get_node(1).mc_by_var:
        tmc = res.get_node(1).mc_by_var[v]
    print(f", {tmc}", end = '')


print("")
res = dDNNF.from_file(cnf_file + ".nnf")
res.annotate_mc()

print(f"{cnf_file}, true, {res.get_node(1).mc}", end = '')
for v in range(1, nb_vars + 1):
    print(f", {res.get_node(1).mc_by_var[v]}", end = '')

print("")
