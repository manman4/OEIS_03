my(N=20, x='x+O('x^N)); concat(0, Vec(serlaplace(sum(k=1, N, k * (1 - exp(-k*x))^k ))))