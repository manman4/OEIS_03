my(N=50, x='x+O('x^N)); concat([0, 0, 0], Vec( 1/(1-x) * sum(k=4, N, x^k/(1-x^k) )))   
