my(N=66, x='x+O('x^N)); Vec(-sum(j=1, N, (1-x^(2*j-1))*sum(k=1, N, (-k)^j*x^(k*(2*j-1))))/(1-x))
