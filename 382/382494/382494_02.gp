my(N=2, M=40, x='x+O('x^M), X=1-x^2-x^3, Y=5); Vec(sum(k=0, (N+1)\2, 4^k*binomial(N+1, 2*k)*X^(N+1-2*k)*x^(Y*k))/(X^2-4*x^Y)^(N+1))

\\ G.f.: (Sum_{k=0..1} 4^k * binomial(3,2*k) * (1-x^2-x^3)^(3-2*k) * x^(5*k)) / ((1-x^2-x^3)^2 - 4*x^5)^3.
my(M=40, x='x+O('x^M)'); Vec( (sum(k=0, 1, 4^k * binomial(3,2*k) * (1-x^2-x^3)^(3-2*k) * x^(5*k))) / ((1-x^2-x^3)^2 - 4*x^5)^3 )



