using Distributions
using DataStructures

include("var.jl")
include("ddnnf.jl")
include("mc.jl")
include("pc.jl")
include("dac.jl")

function exact_uniform_sampling(pdac :: PDAC, N :: Int64)
    tmc = emc(pdac)
    pc = get_pc(pdac.pnnf, 1)

    for _ in 1:N
        id = rand(BigInt(1) : tmc)
        ch = false

        for pid in 1:pc
            s = get_path(pdac.pnnf, pid)
            lunnf = annotate_mc(pdac.unnf, s)
            ai = get_mc(lunnf, 1)

            if id <= ai
                for l in get_solution(lunnf, id)
                    print(mkReadable(l), " ")
                end
                println("0")
                ch = true
                break
            else
                id -= ai
            end
        end

        if !ch
            println("ERROR")
        end
    end
end

function heuristic_uniform_sampling(pdac :: PDAC, N :: Int64, k :: Int64)
    pc = get_pc(pdac.pnnf, 1)

    for _ in 1:N
        pids = Set{BigInt}()
        while length(pids) < k
            push!(pids, rand(BigInt(1):pc))
        end

        pids = collect(pids)
        # mc_pids = collect(map(pids) do pid
        #         s = get_path(dac.pnnf, pid)
        #         lunnf = annotate_mc(dac.unnf, s)
        #         ai = get_mc(lunnf, 1)
        #         ai
        #     end)
        mc_pids = Vector{BigInt}(undef, length(pids))
        Threads.@threads for i in 1:length(pids)
            s = get_path(pdac.pnnf, pids[i])
            lunnf = annotate_mc(pdac.unnf, s)
            ai = get_mc(lunnf, 1)
            mc_pids[i] = ai
        end
        tmc = sum(mc_pids)

        id = rand(BigInt(1) : tmc)
        ch = false

        for i in 1:length(pids)
            if id <= mc_pids[i]
                s = get_path(pdac.pnnf, pids[i])
                lunnf = annotate_mc(pdac.unnf, s)

                for l in get_solution(lunnf, id)
                    print(mkReadable(l), " ")
                end
                println("0")

                ch = true
                break
            else
                id -= mc_pids[i]
            end
        end
    end
end

function get_samples(pdac :: PDAC, N :: Int64, k :: Int64)
    pc = get_pc(pdac.pnnf, 1)

    if pc <= k
        exact_uniform_sampling(pdac, N)
    else
        heuristic_uniform_sampling(pdac, N, k)
    end
end

pdac = pdac_from_file(ARGS[1])
N = parse(Int64, ARGS[2])
k = parse(Int64, ARGS[3])
get_samples(pdac, N, k)
