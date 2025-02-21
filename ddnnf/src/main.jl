using BenchmarkTools
using CairoMakie
using DataFrames
using CSV

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
fc(x) = x.file == ARGS[1]
# data = CSV.read("/home/users/ozeyen/ddnnf/mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = CSV.read("mc.csv", DataFrame, types = Dict("mc" => BigInt))
data = filter(fc, data)
TMC = -1
if length(data.mc) == 1
    global TMC = data.mc[1]
end

println("sp ", get_mc(dac.pnnf, 1))
println("su ", get_mc(dac.unnf, 1))

# Set(sample(dac.pnnf))
# @btime sample(dac.pnnf)

X = Vector{Int64}()
Y = Vector{BigFloat}()

for i in 50:100
    mc = applb(dac, 100, i, 0.1)

    push!(X, i)
    push!(Y, mc)
end

# pmc, amc, vr, X, Y, smc = appmc(dac, 50000, 400)
f = Figure(size = (1000, 1000))
a1 = Axis(f[1, 1])
# a2 = Axis(f[1, 2], title = "ApproxMC", xlabel = "time (s)")
# a3 = Axis(f[2, 1], title = "hp - lp", xlabel = "k")
# a4 = Axis(f[2, 2], title = "epsilon estimate", xlabel = "k (|R_P| = $pmc)")
# 
# uniformp = BigFloat(1) / BigFloat(TMC)
# 
# hist!(a1, vr, bins = 100)
# scatterlines!(a2, X, Y, marker = :cross, strokewidth = 0, markersize = 5)
scatterlines!(a1, X, Y, marker = :cross, strokewidth = 0, markersize = 5)
if TMC != -1
    scatterlines!(a1, [X[begin], X[end]], [TMC, TMC])
end
save(ARGS[1] * ".png", f)
# 
# smcx = Vector{Int64}()
# smcy = Vector{BigFloat}()
# # py = Vector{BigFloat}()
# 
# high = BigInt(0)
# low = BigInt(0)
# for i in 1:length(smc)
#     global high += smc[length(smc) + 1 - i]
#     global low += smc[i]
# 
#     if i >= 50
#         push!(smcx, i)
#         l = (BigFloat(i) / BigFloat(low)) / pmc
#         h = (BigFloat(i) / BigFloat(high)) / pmc
#         push!(smcy, l - h)
# 
#         # if TMC != -1
#         #     # md = max(abs(uniformp - l), abs(uniformp - h))
#         #     ell = (BigFloat(1) / (BigFloat(TMC) * l)) - 1
#         #     elh = (BigFloat(TMC) * l) - 1
#         #     ehl = (BigFloat(1) / (BigFloat(TMC) * h)) - 1
#         #     ehh = (BigFloat(TMC) * h) - 1
#         #     push!(py, max(ell, elh, ehl, ehh))
#         # end
# 
#         if i <= length(smc) / 2 && i + 1 >= length(smc) / 2
#             println(i)
#             lmc = pmc * BigFloat(low) / i
#             hmc = pmc * BigFloat(high) / i
# 
#             println(lmc)
#             println(hmc)
# 
#             scatterlines!(a2, [X[begin], X[end]], [lmc, lmc])
#             scatterlines!(a2, [X[begin], X[end]], [hmc, hmc])
#         end
#     end
# end
# 
# mie, mae, pp = appmc2(dac, 50, 1000)
# println("mie ", minimum(pp))
# println("mae ", maximum(pp))
# 
# scatterlines!(a2, [X[begin], X[end]], [mie, mae], color = :red)
# 
# if TMC != -1
#     scatterlines!(a2, [X[begin], X[end]], [TMC, TMC], color = :black)
#     # scatterlines!(a4, smcx, py, marker = :cross, strokewidth = 0, markersize = 5)
# 
#     # ppy1 = max((1 / (TMC * mapp)) - 1, (TMC * mapp) - 1)
#     # ppy2 = max((1 / (TMC * mipp)) - 1, (TMC * mipp) - 1)
#     ppy = map(maximum, zip(1 ./ (TMC .* pp) .- 1, (TMC .* pp) .- 1))
#     # scatterlines!(a4, [50, 200], [ppy1, ppy2], color = :red)
#     hist!(a4, ppy, bins = 100)
# 
#     println("el ", minimum(ppy))
#     println("eh ", maximum(ppy))
# end
# 
# # scatterlines!(a2, [X[begin], X[end]], [amc2, amc2], color = :red)
# 
# scatterlines!(a3, smcx, smcy, marker = :cross, strokewidth = 0, markersize = 5)
# 
# 
# save(ARGS[1] * ".png", f)
# println("s ", amc)
# 
# # nnf = ddnnf_from_file("res.nnf")
# # annotate_mc(nnf)
# # println("s ", get_mc(nnf, 1))
# # compute_free_var(nnf)
# #
# # @btime nnf = parse("res.nnf")
# # @btime annotate_mc(nnf)
