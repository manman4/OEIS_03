my(N=100, q='q+O('q^N)); Vec( (1/9) * sum(k=4, N, (-1)^k * (2*k+1) * binomial(k+4,8) * q^(k*(k+1)/2)) / sum(k=0, N, (-1)^k * (2*k+1) * q^(k*(k+1)/2)) ) 


