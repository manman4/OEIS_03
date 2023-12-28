a(n) = sum(k=0, n, n^(n-k)*binomial(n+k, k));
for(n=0, 18, print1(a(n), ", "));  
