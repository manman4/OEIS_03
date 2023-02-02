my(N=30, x='x+O('x^N)); Vec(1/sqrt(1-4*x/(1-x^2)))

\\ (n-1)\2はダメ
a(n) = sum(k=0, n\2, binomial(n-1-k, k)*binomial(2*n-4*k, n-2*k));
for(n=0, 15, print1(a(n), ", "));  

