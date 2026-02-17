#include "var.hpp"

Literal::Literal (Variable v) : l((v.get() << 1)) {
}

Literal::Literal (Variable v, int sign) : l((v.get() << 1) | (sign < 0 ? 1 : 0)) {
}

Literal::Literal (int i) : l(((std::abs(i) - 1) << 1) ^ (i < 0 ? 1 : 0)) {
}

Variable::Variable(int i) : v(std::abs(i) - 1) {
}

Variable::Variable(Literal l) : v(l.get() >> 1) {
}

bool Variable::operator == (Variable const& p) const {
    return v == p.v;
}

bool Variable::operator != (Variable const& p) const {
    return v != p.v;
}

bool Variable::operator <  (Variable const& p) const {
    return v < p.v;
}

LitSet::LitSet(std::size_t const nb_lits) : set(nb_lits, 0) {
}

LitSet::LitSet() : set(0, 0) {
}

void LitSet::insert(Literal const& l) {
    if(l.get() >= static_cast<int>(set.size())) {
        set.resize(std::max(l.get(), (~l).get()) + 1);
    }
    set.set(l.get(), true);
}

void LitSet::remove(Literal const& l) {
    if(l.get() < static_cast<int>(set.size())) {
        set.set(l.get(), false);
    }
}

void LitSet::reset() {
    set.reset();
}

bool LitSet::contains(Literal const& l) const {
    return l.get() < static_cast<int>(set.size()) && set.test(l.get());
}

void LitSet::resize(std::size_t const nb_lits) {
    set.resize(nb_lits);
}

bool operator<(LitSet const& l, LitSet const& r) {
    return l.set < r.set;
}

bool operator==(LitSet const& l, LitSet const& r) {
    return l.set == r.set;
}
