class PartitionInfo:
    def __init__(self):
        self.part = {}
        self.part_sizes = {}
        self.files = {}

def from_file(path):
    res = PartitionInfo()

    with open(path, 'r') as file:
        for line in file:
            line = line.strip()

            if line.startswith("v cross "):
                pass
            elif line.startswith("v "):
                tmp = [int(x) for x in line[2:].split(' ')]
                if tmp[1] in res.part:
                    res.part[tmp[1]].add(tmp[0])
                else:
                    res.part[tmp[1]] = {tmp[0]}

            elif line.startswith("c p "):
                tmp = [int(x) for x in line[4:].split(' ')]
                res.part_sizes[tmp[0]] = tmp[1]

            elif line.startswith("p "):
                tmp = line[2:].split(' ')
                p = int(tmp[0])
                res.files[p] = (" ".join(tmp[1:])).strip()
    return res

