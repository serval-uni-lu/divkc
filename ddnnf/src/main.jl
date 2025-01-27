# using BenchmarkTools

include("ddnnf.jl")

nnf = parse(ARGS[1])
annotate_mc(nnf)
println("s ", get_mc(nnf, 1))
compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
