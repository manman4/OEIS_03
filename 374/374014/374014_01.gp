my(N=50, x='x+O('x^N)); Vec(sum(k=0, sqrtnint(N, 4), x^k^4)^16)

a(n) = my(x='x+O('x^(n+1))); polcoef(sum(k=0, sqrtnint(n, 4), x^k^4)^16, n);
for(n=0, 15000, if(a(n)==0, print1(n, ", ")))