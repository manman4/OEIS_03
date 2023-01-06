default(parisize, 120000000);
my(N=131, x='x+O('x^N)); Vec(sum(k=1, N, x^k/(1-x^k^3)))