M=15;

\\ G.f.: (1/2) * Sum_{k>=0} Product_{j=0..k-1} (1/2 + j*x).
my(N=M, x='x+O('x^N)); Vec( (1/2) * sum(k=0, 500, prod(j=0, k-1, (1/2.) + j*x)) )
my(N=M, x='x+O('x^N)); apply(round, Vec( (1/2) * sum(k=0, 500, prod(j=0, k-1, (1/2.) + j*x)) ))