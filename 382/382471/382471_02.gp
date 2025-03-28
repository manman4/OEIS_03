my(N=4, M=30, x='x+O('x^M), X=1-x-x^2, Y=3); Vec(sum(k=0, (N+1)\2, 4^k*binomial(N+1, 2*k)*X^(N+1-2*k)*x^(Y*k))/(X^2-4*x^Y)^(N+1))

\\ G.f.: (Sum_{k=0..2} 4^k * binomial(5,2*k) * (1-x-x^2)^(5-2*k) * x^(3*k)) / ((1-x-x^2)^2 - 4*x^3)^5.
my(M=30, x='x+O('x^M)'); Vec( (sum(k=0, 2, 4^k*binomial(5, 2*k)*(1-x-x^2)^(5-2*k)*x^(3*k)) / ((1-x-x^2)^2 - 4*x^3)^5) )

