import PartitionInfo


res = PartitionInfo.from_file("../splitter/log")
print(res.part)
print(res.part_sizes)
print(res.files)
