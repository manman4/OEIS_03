a089627(n, k) = n!/((n-2*k)!*k!^2);
my(N=4, M=30, x='x+O('x^M), X=1-x-x^2, Y=3); Vec(sum(k=0, N\2, a089627(N, k)*X^(N-2*k)*x^(Y*k))/(X^2-4*x^Y)^(N+1/2))

\\ G.f.: (Sum_{k=0..2} A089627(4,k) * (1-x-x^2)^(4-2*k) * x^(3*k)) / ((1-x-x^2)^2 - 4*x^3)^(9/2).
my(M=30, x='x+O('x^M)); Vec( sum(k=0, 2, a089627(4, k) * (1-x-x^2)^(4-2*k) * x^(3*k)) / ((1-x-x^2)^2 - 4*x^3)^(9/2) )

