my(N=40, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, k*x^k/(1-x^k))^3))

my(N=40, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, x^k/(1-x^k)^2)^3))