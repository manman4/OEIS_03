M=30;

a(n) = numerator(sum(k=0, n-1, binomial(n-1, k)/(k+n)!));
for(n=1, M, print1(a(n), ", "));