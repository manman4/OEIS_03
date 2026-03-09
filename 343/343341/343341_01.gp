my(N=100, x='x+O('x^N)); Vec( 1/prod(k=1,N, 1-x^k) - sum(k=1, N, x^k/prod(d=1, k, if(k%d==0, 1-x^d, 1))) )
