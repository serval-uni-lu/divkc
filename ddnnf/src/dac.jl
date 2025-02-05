struct DAC
    pvar :: Set{Var}
    pnnf :: DDNNF
    unnf :: DDNNF
end

function dac_from_file(path :: String)
    vp = Set{Var}()

    # parse log file
    for line in eachline(path * ".log")
        if startswith(line, "c p show ")
            tmp = map(strip, split(line[length("c p show "):end]))

            toint(x) = Base.parse(Int64, x)
            nonzero(x) = x != 0

            y = map(mkVar, filter(nonzero, map(toint, tmp)))
            union!(vp, y)
        end
    end

    # proj ddnnf
    pnnf = ddnnf_from_file(path * ".pnnf", true, vp)
    annotate_mc(pnnf)

    # upper bound ddnnf
    unnf = ddnnf_from_file(path * ".unnf")
    annotate_mc(unnf)

    return DAC(vp, pnnf, unnf)
end

function appmc(dac :: DAC, N :: Int64)
    sigma = BigInt(0)
    vr = Vector{BigInt}()
    for i in 1:N
        s = Set(sample(dac.pnnf))
        annotate_mc(dac.unnf, s)
        ai = get_mc(dac.unnf, 1)
        push!(vr, ai)
        sigma += ai
    end

    println("s ", get_mc(dac.pnnf, 1) * sigma / N)
    return vr
end
