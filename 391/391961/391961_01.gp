M=30;

\\ G.f.: ((1-x)^2 + x^3) / ((1-x)^2 - x^3)^2.
my(N=M, x='x+O('x^N)); Vec( ((1-x)^2 + x^3) / ((1-x)^2 - x^3)^2 )

my(A=2, B=3, C=A^2*B, N=7, M=40, x='x+O('x^M), X=1-A*x^3, Y=7); Vec(sum(k=0, N\2, C^k*binomial(N, 2*k)*X^(N-2*k)*x^(Y*k))/(X^2-C*x^Y)^N)