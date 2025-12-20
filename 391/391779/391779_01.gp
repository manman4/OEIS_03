M=30;

\\ G.f.: (Sum_{k=0..3} 48^k * binomial(7,2*k) * (1-2*x^3-6*x^4)^(7-2*k) * x^(7*k)) / ((1-2*x^3-6*x^4)^2 - 48*x^7)^7.
my(N=M, x='x+O('x^N)); Vec( sum(k=0, 3, 48^k * binomial(7,2*k) * (1-2*x^3-6*x^4)^(7-2*k) * x^(7*k)) / ((1-2*x^3-6*x^4)^2 - 48*x^7)^7 )

my(A=2, B=3, C=4*A^2*B, N=7, M=30, x='x+O('x^M), X=1-A*x-A*B*x^2, Y=3); Vec(sum(k=0, N\2, C^k*binomial(N, 2*k)*X^(N-2*k)*x^(Y*k))/(X^2-C*x^Y)^N)