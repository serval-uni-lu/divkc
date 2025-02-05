using BenchmarkTools
using CairoMakie

include("var.jl")
include("ddnnf.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
println("sp ", get_mc(dac.pnnf, 1))
println("su ", get_mc(dac.unnf, 1))

# Set(sample(dac.pnnf))
# @btime sample(dac.pnnf)

vr = appmc(dac, 1000)
f = Figure()
hist(f[1, 1], vr, bins = 25)
save(ARGS[1] * ".png", f)

# nnf = ddnnf_from_file("res.nnf")
# annotate_mc(nnf)
# println("s ", get_mc(nnf, 1))
# compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
