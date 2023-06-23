my(N=90, x='x+O('x^N)); concat(0, Vec(sum(k=1, N, [1, 0, 1, 0, 0, 1, 0][k%7+1]*k*x^k/(1-x^k))))

