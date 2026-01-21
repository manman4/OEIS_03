M=30;

\\ Expansion of Sum_{k>=0} x^(2*k) / Product_{j=1..3*k} (1 - j * x).
my(N=M, x='x+O('x^N)); Vec( sum(k=0, N, x^(2*k) / prod(j=1, 3*k, (1 - j*x))) )