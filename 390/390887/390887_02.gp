M=15;

\\ G.f.: Sum_{k>=0} (3*x)^k / Product_{j=1..k} (1 - 2*j*x).
my(N=M, x='x+O('x^N)); Vec( sum(k=0, N, (3*x)^k / prod(j=1, k, 1 - 2*j*x)) )