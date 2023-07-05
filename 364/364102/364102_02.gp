my(N=90, x='x+O('x^N)); concat([0, 0], Vec(sum(k=1, N, (5*k-1) * x^(3*k) / (1 - x^(5*k-1)) )))
 