my(N=66, x='x+O('x^N)); concat([0, 0, 0, 0], Vec(sum(k=1, sqrtint(N\5), x^(5*k^2)/prod(j=1, 5*k-1, 1-x^j))))
