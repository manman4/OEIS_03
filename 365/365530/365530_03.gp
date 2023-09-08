my(N=20, x='x+O('x^N)); concat([0, 0], Vec(sum(k=0, N, x^(5*k+2)/prod(j=1, 5*k+2, 1-j*x))))  
 