\\ G.f.: Sum_{k>=1} q^k * B(q^k), where B(q) is the g.f. of A000009.
my(N=70, q='q+O('q^N)); Vec(sum(k=1, N, q^k * subst(prod(j=1, N, 1+q^j), 'q, q^k)))