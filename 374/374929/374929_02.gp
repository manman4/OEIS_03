my(N=201, x='x+O('x^N)); Vec(sum(i=1, N, sum(j=i, N, x^(i+j)/( (1-x^i)*(1-x^j) )^2 )))

