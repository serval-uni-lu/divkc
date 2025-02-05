# using BenchmarkTools

include("var.jl")
include("ddnnf.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
println(get_mc(dac.pnnf, 1))
println(get_mc(dac.unnf, 1))
# nnf = ddnnf_from_file("res.nnf")
# annotate_mc(nnf)
# println("s ", get_mc(nnf, 1))
# compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
