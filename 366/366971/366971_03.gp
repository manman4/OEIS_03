my(N=50, x='x+O('x^N)); concat([0, 0], Vec( 1/(1-x) * sum(k=3, N, binomial(k,3) * x^k/(1-x^k) )))   
