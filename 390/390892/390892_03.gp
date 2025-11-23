M=15;

\\ G.f.: (1/4) * Sum_{k>=0} (3/4)^k / Product_{j=1..k} (1 - j*x).
my(N=M, x='x+O('x^N)); Vec( (1/4) * sum(k=0, 500, (3/4.)^k / prod(j=1, k, 1 - j*x)) )
my(N=M, x='x+O('x^N)); apply(round, Vec( (1/4) * sum(k=0, 500, (3/4.)^k / prod(j=1, k, 1 - j*x)) ))