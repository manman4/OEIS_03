my(N=50, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, x^(2*k)/(1-k*x^k)^2)))