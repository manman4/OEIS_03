my(N=20, x='x+O('x^N)); Vec(sum(j=1, N, (1-x^j)*sum(k=1, N, (k*x^j)^k))/(1-x))
