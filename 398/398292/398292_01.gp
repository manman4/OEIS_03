\\ a(n) = Sum_{k=0..n} (4*k)!/k!^4 * binomial(k,n-k).
a(n) = sum(k=0, n, (4*k)!/k!^4*binomial(k, n-k));
for(n=0, 14, print1(a(n),", "));  
