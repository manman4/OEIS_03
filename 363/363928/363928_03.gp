my(N=90, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, k * x^(5*k-2) / (1 - x^(5*k-2)) )))

