M=15;

\\ G.f.: (1/3) * Sum_{k>=0} (2/3)^k / Product_{j=1..k} (1 - j*x).
my(N=M, x='x+O('x^N)); Vec( (1/3) * sum(k=0, 500, (2/3.)^k / prod(j=1, k, 1 - j*x)) )
my(N=M, x='x+O('x^N)); apply(round, Vec( (1/3) * sum(k=0, 500, (2/3.)^k / prod(j=1, k, 1 - j*x)) ))