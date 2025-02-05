using BenchmarkTools

include("var.jl")
include("ddnnf.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
println("sp ", get_mc(dac.pnnf, 1))
println("su ", get_mc(dac.unnf, 1))

# Set(sample(dac.pnnf))
# @btime sample(dac.pnnf)

appmc(dac, 1000)

# nnf = ddnnf_from_file("res.nnf")
# annotate_mc(nnf)
# println("s ", get_mc(nnf, 1))
# compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
