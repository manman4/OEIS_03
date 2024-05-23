my(N=70, x='x+O('x^N)); Vec(sum(k=0, N, x^(4*k*(4*k+1)/2) / prod(j=1, 4*k, 1-x^j)))

my(N=70, x='x+O('x^N)); Vec(sum(k=0, N, prod(j=1, 4*k, x^j/(1-x^j))))