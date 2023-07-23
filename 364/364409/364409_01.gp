a(n) = if(n==0, 1, (-1)^(n-1)*sum(k=0, n, binomial(n, k)*binomial(n+5*k-2, n-1))/n);
for(n=0, 18, print1(a(n),", "))  