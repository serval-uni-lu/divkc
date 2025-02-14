struct ADDNNF
    nnf :: DDNNF
    assumps :: Set{Lit}
    mc :: Vector{BigInt}
end

function get_mc(nnf :: ADDNNF, e :: Edge)
    for l in get_literals(nnf.nnf, e)
        if (~l) in nnf.assumps
            return 0
        end
    end
    nfree = e.e_free - e.b_free + 1
    for v in get_free_vars(nnf.nnf, e)
        l = mkLit(v, false)
        if (l in nnf.assumps) || ((~l) in nnf.assumps)
            nfree -= 1
        end
    end
    return get_mc(nnf, e.child) * BigInt(2)^(nfree)
end

get_mc(nnf :: ADDNNF, i :: Int64) = nnf.mc[i]

function annotate_mc(nnf :: DDNNF, assumps :: Set{Lit} = Set{Lit}())
    res = ADDNNF(nnf, assumps, Vector{BigInt}(undef, length(nnf.nodes)))

    for i in nnf.ordering
        annotate_mc(res, i, nnf.nodes[i])
    end

    return res
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: FalseNode)
    nnf.mc[i] = 0
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: TrueNode)
    nnf.mc[i] = 1
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: UnaryNode)
    nnf.mc[i] = get_mc(nnf, n.child)
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: OrNode)
    nnf.mc[i] = 0

    for c in n.children
        nnf.mc[i] += get_mc(nnf, c)
    end
end

function annotate_mc(nnf :: ADDNNF, i :: Int64, n :: AndNode)
    nnf.mc[i] = 1

    for c in n.children
        nnf.mc[i] *= get_mc(nnf, c)
    end
end



function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: TrueNode)
    return []
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: UnaryNode)
    for l in get_literals(nnf.nnf, n.child)
        push!(s, l)
    end
    for v in get_free_vars(nnf.nnf, n.child)
        push!(s, mkLit(v, rand((true, false))))
    end
    return [n.child.child]
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: OrNode)
    x = rand(BigInt(1):get_mc(nnf, i))
    for c in n.children
        cmc = get_mc(nnf, c)

        if x <= cmc
            for l in get_literals(nnf.nnf, c)
                push!(s, l)
            end
            for v in get_free_vars(nnf.nnf, c)
                push!(s, mkLit(v, rand((true, false))))
            end
            return [c.child]
        else
            x -= cmc
        end
    end
end

function sample(nnf :: ADDNNF, s :: Vector{Lit}, i :: Int64, n :: AndNode)
    f(c) = c.child
    return map(f, n.children)
end

# only works if annotate_mc has been called with assumps an empty set
function sample(nnf :: ADDNNF)
    stack = [Int64(1)]
    res = Vector{Lit}()

    while length(stack) > 0
        x = pop!(stack)
        y = sample(nnf, res, x, nnf.nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    return res
end
