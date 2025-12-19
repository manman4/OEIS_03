M=30;

\\ G.f.: (Sum_{k=0..3} 72^k * binomial(7,2*k) * (1-3*x-6*x^4)^(7-2*k) * x^(5*k)) / ((1-3*x-6*x^4)^2 - 72*x^5)^7.
my(N=M, x='x+O('x^N)); Vec( sum(k=0, 3, 72^k * binomial(7,2*k) * (1-3*x-6*x^4)^(7-2*k) * x^(5*k)) / ((1-3*x-6*x^4)^2 - 72*x^5)^7 )

my(A=2, B=3, C=4*A*B^2, N=1, M=30, x='x+O('x^M), X=1-B*x-A*B*x^4, Y=5); Vec(sum(k=0, N\2, C^k*binomial(N, 2*k)*X^(N-2*k)*x^(Y*k))/(X^2-C*x^Y)^N)