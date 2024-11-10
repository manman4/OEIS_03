a(n) = apply(round, sum(k=1, n, tan(k*Pi/(1+2*n))^4));
for(n=0, 40, print1(a(n), ", "))


