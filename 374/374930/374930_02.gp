my(N=55, x='x+O('x^N)); Vec(sum(i=1, N, sum(j=i, N, sum(k=j, N, x^(i+j+k)/( (1-x^i)*(1-x^j)*(1-x^k) )^2 ))))

