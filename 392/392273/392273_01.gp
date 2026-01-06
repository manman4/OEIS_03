M=40;

\\ G.f.: (1-x^3)^2 / ((1-x^3)^3 - x^10).
my(N=M, x='x+O('x^N)); Vec( (1 - x^3)^2 / ((1 - x^3)^3 - x^10) )

a178618(n, k) = sum(j=0, k, (-1)^(k-j)*binomial(n+1, k-j)*binomial(n+3*j, 3*j));
my(A=1, B=1, C=A^3*B, N=1, M=60, x='x+O('x^M), X=1-A*x^3, Y=10); Vec(sum(k=0, (2*N)\3, C^k*a178618(N-1, k)*X^(2*N-3*k)*x^(Y*k))/(X^3-C*x^Y)^N)