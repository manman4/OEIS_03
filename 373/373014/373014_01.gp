my(N=60, x='x+O('x^N)); concat(0, Vec(sum(k=0, N, x^(3*k+1)/prod(j=1, 3*k+1, 1-x^j))))
