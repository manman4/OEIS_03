my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(x * sum(k=0, N, binomial(3*k,k)/(2*k+1) * (2*x)^k)^2 ) ))