\\ E.g.f.: exp(exp(x) - 1) * Sum_{k=0..5} Stirling2(6,k+1) * (exp(x) - 1)^k.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(exp(x) - 1) * sum(k=0, 5, stirling(6,k+1,2) * (exp(x)-1)^k )))
