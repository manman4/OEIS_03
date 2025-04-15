\\ E.g.f.: exp(exp(x) - 1) * Sum_{k=0..4} Stirling2(5,k+1) * (exp(x) - 1)^k.
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(exp(x) - 1) * sum(k=0, 4, stirling(5,k+1,2) * (exp(x)-1)^k )))

