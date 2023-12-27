a(n) = sum(k=0, n, n^k*binomial(k+n-1, k));
for(n=0, 16, print1(a(n), ", "));  
