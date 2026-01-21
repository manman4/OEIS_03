M=30;

\\ Expansion of Sum_{k>=0} x^k / Product_{j=1..3*k} (1 - j * x).
my(N=M, x='x+O('x^N)); Vec( sum(k=0, N, x^k / prod(j=1, 3*k, (1 - j*x))) )