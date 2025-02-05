struct Lit
    x :: Int32
end

struct Var
    x :: Int32
end

mkVar(x :: Integer) = Var(Int32(abs(x) - 1))
mkVar(x :: Lit) = Var(x.x >> 1)
mkVar(x :: Var) = x

mkLit(x :: Integer) = mkLit(mkVar(x), x < 0)
mkLit(x :: Var, s :: Bool) = Lit(x.x << 1 | (s ? 1 : 0))
mkLit(x :: Lit) = x

toIndex(x :: Var) = x.x + 1
toIndex(x :: Lit) = x.x + 1

mkReadable(x :: Var) = x.x + 1
mkReadable(x :: Lit) = ((x.x >> 1) + 1) * (x.x & 1 == 1 ? -1 : 1)
