my(N=50, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, binomial(k+3,5) * x^k/(1-x^k))))