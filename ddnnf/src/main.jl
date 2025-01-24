# using BenchmarkTools

include("ddnnf.jl")

get_mc(n) = n.mc
get_mc(nnf :: DDNNF, i :: Int64) = get_mc(nnf.nodes[i])

nnf = parse("res.nnf")
annotate_mc(nnf)
println(get_mc(nnf, 1))
# compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
