struct DAC
    pvar :: Set{Var}
    pnnf :: DDNNF
    unnf :: DDNNF
end

struct PDAC
    pvar :: Set{Var}
    pnnf :: DDNNF
    unnf :: DDNNF
end

function dac_from_file(path :: String)
    vp = Set{Var}()

    # parse log file
    for line in eachline(path * ".proj.log")
        if startswith(line, "c p show ")
            tmp = map(strip, split(line[length("c p show "):end]))

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0

            y = map(mkVar, filter(nonzero, map(toint, tmp)))
            union!(vp, y)
        end
    end

    pnnf = ddnnf_from_file(path * ".pnnf", true, vp)
    # pnnf = ddnnf_from_file(path * ".pnnf")
    annotate_mc(pnnf)

    # upper bound ddnnf
    unnf = ddnnf_from_file(path * ".unnf")
    # annotate_mc(unnf)

    return DAC(vp, pnnf, unnf)
end

function pdac_from_file(path :: String)
    vp = Set{Var}()

    # parse log file
    for line in eachline(path * ".proj.log")
        if startswith(line, "c p show ")
            tmp = map(strip, split(line[length("c p show "):end]))

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0

            y = map(mkVar, filter(nonzero, map(toint, tmp)))
            union!(vp, y)
        end
    end

    pnnf = ddnnf_from_file(path * ".pnnf", true, vp)
    # pnnf = ddnnf_from_file(path * ".pnnf")
    annotate_pc(pnnf)

    # upper bound ddnnf
    unnf = ddnnf_from_file(path * ".unnf")
    # aunnf = annotate_mc(unnf)

    return PDAC(vp, pnnf, unnf)
end

function cleanup(smc :: Dict{BigInt, BigInt}, k :: Int64)
    f(x) = x[2]
    y = collect(smc)
    sort!(y, by = f)

    tl = y[k][2]
    th = y[end-k][2]

    p(x) = x[2] <= tl || x[2] >= th
    filter!(p, smc)
end

function map_model(m :: Set{Lit})
    m = collect(m)
    sort!(m, by = toIndex)

    res = BigInt(0)
    for i in m
        res <<= 1
        if sign(i) == 1
            res += 1
        end
    end
    return res
end

function appmc(dac :: DAC, N :: Int64)
    Y = Vector{BigFloat}()
    L = Vector{BigInt}()
    S = Vector{BigFloat}()

    total = BigInt(0)
    z = quantile(Normal(), 1 - 0.05)

    for i in 1:N
        s = Set(sample(dac.pnnf))
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1) * get_mc(dac.pnnf, 1)


        li = length(Y) + 1
        total += ai
        m = total / li
        push!(L, ai)

        var = sum((L .- m) .^ 2) / (li - 1)
        sd = sqrt(var) / sqrt(li)

        push!(Y, m)
        push!(S, sd)
    end

    return Y, Y .- z .* S, Y .+ z .* S
end

function appmc(dac :: PDAC, N :: Int64)
    Y = Vector{BigFloat}()
    L = Vector{BigInt}()
    S = Vector{BigFloat}()

    total = BigInt(0)
    z = quantile(Normal(), 1 - 0.01)

    for i in 1:N
        s = get_path(dac.pnnf, rand(BigInt(1) : get_pc(dac.pnnf, 1)))
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1) * get_pc(dac.pnnf, 1)


        li = length(Y) + 1
        total += ai
        m = total / li
        push!(L, ai)

        var = sum((L .- m) .^ 2) / (li - 1)
        sd = sqrt(var) / sqrt(li)

        push!(Y, m)
        push!(S, sd)
    end

    return Y, Y .- z .* S, Y .+ z .* S
end

# Welford's online algorithm
function appmc_l(dac :: PDAC, N :: Int64)
    rmean = BigFloat(0)
    rm = BigFloat(0)
    k = 0

    z = quantile(Normal(), 1 - 0.01)

    for i in 1:N
        s = get_path(dac.pnnf, rand(BigInt(1) : get_pc(dac.pnnf, 1)))
        # s = Set(get_solution(dac.pnnf, id))
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1) * get_pc(dac.pnnf, 1)

        k += 1
        n_mean = rmean + ((ai - rmean) / k)
        rm = rm + ((ai - rmean) * (ai - n_mean))
        rmean = n_mean
    end

    S2 = rm / (k - 1)
    sd = sqrt(S2) / sqrt(k)
    return rmean, rmean - z * sd, rmean + z * sd
end

function emc(dac :: DAC)
    mc = get_mc(dac.pnnf, 1)
    sigma = BigInt(0)

    for i in BigInt(1):mc
        s = Set(get_solution(dac.pnnf, i))
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1)

        sigma += ai
    end

    return sigma
end

function emc(dac :: PDAC)
    mc = get_pc(dac.pnnf, 1)
    sigma = BigInt(0)

    for i in BigInt(1):mc
        s = get_path(dac.pnnf, i)
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1)

        sigma += ai
    end

    return sigma
end
