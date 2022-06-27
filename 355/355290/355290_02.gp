M=25;

my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, binomial(2*k, k)/(k+1)*x^k/prod(j=1, k, 1+j*x)))