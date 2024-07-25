my(N=55, x='x+O('x^N)); Vec(sum(i=1, N, sum(j=i, N, sum(k=j, N, sum(l=k, N, x^(i+j+k+l)/( (1-x^i)*(1-x^j)*(1-x^k)*(1-x^l) )^2 )))))

