M=50;

\\ G.f.: (1-x^2) / ((1-x^2)^2 - x^5).
my(N=M, x='x+O('x^N)); Vec( (1 - x^2) / ((1 - x^2)^2 - x^5) )

my(A=1, B=1, C=A^2*B, N=1, M=50, x='x+O('x^M), X=1-A*x^2, Y=5); Vec(sum(k=0, N\2, C^k*binomial(N, 2*k)*X^(N-2*k)*x^(Y*k))/(X^2-C*x^Y)^N)