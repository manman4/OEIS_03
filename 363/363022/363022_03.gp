my(N=60, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, binomial(k,2) * (-x)^k/(1-x^k))))