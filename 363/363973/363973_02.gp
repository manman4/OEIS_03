my(N=90, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, k^2 * x^(4*k-1) / (1 - x^(4*k-1)) )))

