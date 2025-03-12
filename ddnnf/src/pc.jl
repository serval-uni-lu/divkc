struct PCDDNNF
    nnf :: DDNNF
    assumps :: Set{Lit}
    pc :: Vector{BigInt}
end

function get_pc(nnf :: PCDDNNF, e :: Edge)
    for l in get_literals(nnf.nnf, e)
        if (~l) in nnf.assumps
            return 0
        end
    end

    return get_pc(nnf, e.child)
end

get_pc(nnf :: PCDDNNF, i :: Int64) = nnf.pc[i]

function annotate_pc(nnf :: DDNNF, assumps :: Set{Lit} = Set{Lit}())
    res = PCDDNNF(nnf, assumps, Vector{BigInt}(undef, length(nnf.nodes)))

    for i in nnf.ordering
        annotate_pc(res, i, nnf.nodes[i])
    end

    return res
end

function annotate_pc(nnf :: PCDDNNF, i :: Int64, n :: FalseNode)
    nnf.pc[i] = 0
end

function annotate_pc(nnf :: PCDDNNF, i :: Int64, n :: TrueNode)
    nnf.pc[i] = 1
end

function annotate_pc(nnf :: PCDDNNF, i :: Int64, n :: UnaryNode)
    nnf.pc[i] = get_pc(nnf, n.child)
end

function annotate_pc(nnf :: PCDDNNF, i :: Int64, n :: OrNode)
    f(x) = get_pc(nnf, x)
    nnf.pc[i] = sum(f, n.children)
end

function annotate_pc(nnf :: PCDDNNF, i :: Int64, n :: AndNode)
    f(x) = get_pc(nnf, x)
    nnf.pc[i] = prod(f, n.children)
end

function get_path(nnf :: PCDDNNF, s :: Set{Lit}, id :: BigInt, i :: Int64, n :: TrueNode)
    return []
end

function get_path(nnf :: PCDDNNF, s :: Set{Lit}, id :: BigInt, i :: Int64, n :: FalseNode)
    return []
end

function get_path(nnf :: PCDDNNF, s :: Set{Lit}, id :: BigInt, i :: Int64, n :: UnaryNode)
    union!(s, get_literals(nnf.nnf, n.child))
    if id > get_pc(nnf, n.child.child)
        println("u ", id, " >= ", get_pc(nnf, n.child.child))
    end
    return [(id, n.child.child)]
end

function get_path(nnf :: PCDDNNF, s :: Set{Lit}, id :: BigInt, i :: Int64, n :: OrNode)
    if id > get_pc(nnf, i)
        println("o ", id, " <= ", get_pc(nnf, i))
    end
    for c in n.children
        cmc = get_pc(nnf, c)

        if id <= cmc
            union!(s, get_literals(nnf.nnf, c))

            if id > get_pc(nnf, c.child)
                println("- o ", id, " >= ", get_pc(nnf, c.child))
            end

            return [(id, c.child)]
        else
            id -= cmc
        end
    end

    # return [(id, n.child.child)]
end

function get_path(nnf :: PCDDNNF, s :: Set{Lit}, id :: BigInt, i :: Int64, n :: AndNode)
    res = Vector{Tuple{BigInt, Int64}}()

    tmc = get_pc(nnf, i)
    if id > tmc
        println("a ", id, " <= ", tmc)
    end
    for c in n.children
        cmc = get_pc(nnf, c)

        x = ((id - 1) % cmc) + 1
        y = div(id - 1, cmc) + 1

        if x > cmc
            println("- a ", id, " <= ", tmc)
        end

        push!(res, (x, c.child))
        id = y
    end

    return res
end

function get_path(nnf :: PCDDNNF, id :: BigInt)
    stack = [(id, Int64(1))]
    res = Set{Lit}()

    while length(stack) > 0
        lid, x = pop!(stack)
        y = get_path(nnf, res, lid, x, nnf.nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    for l in nnf.assumps
        push!(res, l)
    end

    return res
end
