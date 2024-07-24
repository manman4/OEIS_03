my(N=201, x='x+O('x^N)); Vec(sum(i=1, N\3, sum(j=i, N-i, sum(k=j, N-i-j, x^(i+j+k)/( (1-x^i)*(1-x^j)*(1-x^k) )^2 ))))

