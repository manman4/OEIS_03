\\ a(n) = Sum_{k=0..n} (3*k)!/k!^3 * binomial(2*k,n-k).
a(n) = sum(k=0, n, (3*k)!/k!^3*binomial(2*k, n-k));
for(n=0, 17, print1(a(n),", "));  
