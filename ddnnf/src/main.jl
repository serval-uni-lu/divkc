using BenchmarkTools
using CairoMakie

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
println("sp ", get_mc(dac.pnnf, 1))
println("su ", get_mc(dac.unnf, 1))

# Set(sample(dac.pnnf))
# @btime sample(dac.pnnf)

amc, vr, X, Y, smc = appmc(dac, 10000)
f = Figure(size = (4500, 1500))
a1 = Axis(f[1, 1])
a2 = Axis(f[1, 2])
a3 = Axis(f[1, 3])

hist!(a1, vr, bins = 100)
scatterlines!(a2, X, Y, marker = :cross, strokewidth = 0, markersize = 5)

smcx = Vector{Int64}()
smcyl = Vector{BigFloat}()
smcyh = Vector{BigFloat}()

high = BigInt(0)
low = BigInt(0)
for i in 1:length(smc)
    global high += smc[length(smc) + 1 - i]
    global low += smc[i]

    push!(smcx, i)
    push!(smcyl, BigFloat(1) / BigFloat(low))
    push!(smcyh, BigFloat(1) / BigFloat(high))
end

scatterlines!(a3, smcx, smcyl, marker = :cross, strokewidth = 0, markersize = 5)
scatterlines!(a3, smcx, smcyh, marker = :cross, strokewidth = 0, markersize = 5)

save(ARGS[1] * ".png", f)
println("s ", amc)

# nnf = ddnnf_from_file("res.nnf")
# annotate_mc(nnf)
# println("s ", get_mc(nnf, 1))
# compute_free_var(nnf)
#
# @btime nnf = parse("res.nnf")
# @btime annotate_mc(nnf)
