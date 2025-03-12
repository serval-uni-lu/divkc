function get_pc(nnf :: DDNNF, e :: Edge)
    for l in get_literals(nnf, e)
        if (~l) in nnf.assumps
            return 0
        end
    end

    return get_pc(nnf, nnf.nodes[e.child])
end

get_pc(nnf :: DDNNF, i :: Int64) = get_pc(nnf, nnf.nodes[i])
get_pc(nnf :: DDNNF, n :: UnaryNode) = n.mc
get_pc(nnf :: DDNNF, n :: OrNode) = n.mc
get_pc(nnf :: DDNNF, n :: AndNode) = n.mc
get_pc(nnf :: DDNNF, n :: TrueNode) = BigInt(1)
get_pc(nnf :: DDNNF, n :: FalseNode) = BigInt(0)

function annotate_pc(nnf :: DDNNF, assumps :: Set{Lit} = Set{Lit}())
    nnf.assumps = assumps

    for i in nnf.ordering
        annotate_pc(nnf, nnf.nodes[i])
    end
end

function annotate_pc(nnf :: DDNNF, n :: FalseNode)
end

function annotate_pc(nnf :: DDNNF, n :: TrueNode)
end

function annotate_pc(nnf :: DDNNF, n :: UnaryNode)
    n.mc = get_pc(nnf, n.child)
end

function annotate_pc(nnf :: DDNNF, n :: OrNode)
    f(x) = get_pc(nnf, x)
    n.mc = sum(f, n.children)
end

function annotate_pc(nnf :: DDNNF, n :: AndNode)
    f(x) = get_pc(nnf, x)
    n.mc = prod(f, n.children)
end

function get_path(nnf :: DDNNF, s :: Set{Lit}, id :: BigInt, n :: TrueNode)
    return []
end

function get_path(nnf :: DDNNF, s :: Set{Lit}, id :: BigInt, n :: FalseNode)
    return []
end

function get_path(nnf :: DDNNF, s :: Set{Lit}, id :: BigInt, n :: UnaryNode)
    union!(s, get_literals(nnf, n.child))
    if id > get_pc(nnf, n.child.child)
        println("u ", id, " >= ", get_pc(nnf, n.child.child))
    end
    return [(id, n.child.child)]
end

function get_path(nnf :: DDNNF, s :: Set{Lit}, id :: BigInt, n :: OrNode)
    if id > get_pc(nnf, n)
        println("o ", id, " <= ", get_pc(nnf, i))
    end
    for c in n.children
        cmc = get_pc(nnf, c)

        if id <= cmc
            union!(s, get_literals(nnf, c))

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

function get_path(nnf :: DDNNF, s :: Set{Lit}, id :: BigInt, n :: AndNode)
    res = Vector{Tuple{BigInt, Int64}}()

    tmc = get_pc(nnf, n)
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

function get_path(nnf :: DDNNF, id :: BigInt)
    stack = [(id, Int64(1))]
    res = Set{Lit}()

    while length(stack) > 0
        lid, x = pop!(stack)
        y = get_path(nnf, res, lid, nnf.nodes[x])
        for i in y
            push!(stack, i)
        end
    end

    for l in nnf.assumps
        push!(res, l)
    end

    return res
end
