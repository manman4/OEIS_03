M=15;

\\ G.f.: (x/4) * Sum_{k>=0} Product_{j=0..k-1} ((3/4) * (1 + j*x)).
my(N=M, x='x+O('x^N)); Vec( (x/4) * sum(k=0, 500, prod(j=0, k-1, (3/4)*(1. + j*x))) )
my(N=M, x='x+O('x^N)); apply(round, Vec( (x/4) * sum(k=0, 500, prod(j=0, k-1, (3/4)*(1. + j*x))) ))
