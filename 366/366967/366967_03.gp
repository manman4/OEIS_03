my(N=50, x='x+O('x^N)); concat(0, Vec( 1/(1-x) * sum(k=2, N, binomial(k,2) * x^k/(1-x^k) )))   
