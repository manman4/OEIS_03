\\ a(n) = Sum_{k=0..n} (4*k)!/k!^4 * binomial(5*k,n-k).
a(n) = sum(k=0, n, (4*k)!/k!^4*binomial(5*k, n-k));
for(n=0, 14, print1(a(n),", "));  
