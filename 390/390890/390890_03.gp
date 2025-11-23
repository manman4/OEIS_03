M=15;

\\ G.f.: Sum_{k>=0} (1/2)^(k+1) / Product_{j=1..k} (1 - j*x).
my(N=M, x='x+O('x^N)); Vec( sum(k=0, 500, (1/2.)^(k+1) / prod(j=1, k, 1 - j*x)) )
my(N=M, x='x+O('x^N)); apply(round, Vec( sum(k=0, 500, (1/2.)^(k+1) / prod(j=1, k, 1 - j*x)) ))