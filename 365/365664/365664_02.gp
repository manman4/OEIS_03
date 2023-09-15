my(N=55, x='x+O('x^N)); Vec(sum(i=1, N\4, sum(j=i+1, N, sum(k=j+1, N, sum(l=k+1, N, x^(i+j+k+l)/( (1-x^i)*(1-x^j)*(1-x^k)*(1-x^l) )^2 )))))

