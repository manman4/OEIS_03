\\ E.g.f.: (1+x) * Sum_{k=0..6} Stirling2(7,k+1) * log(1+x)^k.
my(N=30, x='x+O('x^N)); Vec(serlaplace((1+x) * sum(k=0, 6, stirling(7,k+1,2) * log(1+x)^k )))
