my(N=20, x='x+O('x^N)); Vec(serlaplace(exp(sum(k=1, N, numdiv(k)*(exp(x)-1)^k/k))))
