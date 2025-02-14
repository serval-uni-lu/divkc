using BenchmarkTools
using CairoMakie
using DataFrames
using CSV

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("dac.jl")

dac = dac_from_file(ARGS[1])
# fc(x) = x.file == ARGS[1]
# data = CSV.read("mc.csv", DataFrame, types = Dict("mc" => BigInt))
# data = filter(fc, data)
# TMC = -1
# if length(data.mc) == 1
#     global TMC = data.mc[1]
# end
# 
# println("sp ", get_mc(dac.pnnf, 1))
# println("su ", get_mc(dac.unnf, 1))
# 
# # Set(sample(dac.pnnf))
# # @btime sample(dac.pnnf)
# 
# pmc, amc, vr, X, Y, smc = appmc(dac, 1000, 200)
# f = Figure(size = (3000, 3000))
# a1 = Axis(f[1, 1], title = "c_i distribution", xlabel = "c_i")
# a2 = Axis(f[1, 2], title = "ApproxMC", xlabel = "time (s)")
# a3 = Axis(f[2, 1], title = "hp - lp", xlabel = "k")
# a4 = Axis(f[2, 2], title = "epsilon estimate", xlabel = "k (|R_P| = $pmc)")
# 
# uniformp = BigFloat(1) / BigFloat(TMC)
# 
# hist!(a1, vr, bins = 100)
# scatterlines!(a2, X, Y, marker = :cross, strokewidth = 0, markersize = 5)
# 
# smcx = Vector{Int64}()
# smcy = Vector{BigFloat}()
# py = Vector{BigFloat}()
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
#         if TMC != -1
#             # md = max(abs(uniformp - l), abs(uniformp - h))
#             ell = (BigFloat(1) / (BigFloat(TMC) * l)) - 1
#             elh = (BigFloat(TMC) * l) - 1
#             ehl = (BigFloat(1) / (BigFloat(TMC) * h)) - 1
#             ehh = (BigFloat(TMC) * h) - 1
#             push!(py, max(ell, elh, ehl, ehh))
#         end
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
# if TMC != -1
#     scatterlines!(a2, [X[begin], X[end]], [TMC, TMC], color = :black)
# end
# 
# scatterlines!(a3, smcx, smcy, marker = :cross, strokewidth = 0, markersize = 5)
# 
# scatterlines!(a4, smcx, py, marker = :cross, strokewidth = 0, markersize = 5)
# 
# save(ARGS[1] * ".png", f)
# println("s ", amc)

tmp = emc(dac)
println("cmc ", tmp)
println("tmc ", TMC)
