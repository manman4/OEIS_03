my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(x * sum(k=0, N, binomial(4*k,k)/(3*k+1) * (3*x)^k)^3 ) ))

\\ E.g.f. A(x) satisfies x = log(A(x)) * (1 - 3*log(A(x)))^3.
my(N=30, x='x+O('x^N)); serlaplace( x * sum(k=0, N, binomial(4*k,k)/(3*k+1) * (3*x)^k)^3 * (1 - 3 * x * sum(k=0, N, binomial(4*k,k)/(3*k+1) * (3*x)^k)^3)^3 )