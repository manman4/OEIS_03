a(n) = sum(k=0, n, binomial(5*k+1,k) * binomial(k,n-k)/(5*k+1) );
for(n=0, 20, print1(a(n), ", "))

b(n) = sum(k=0, n, binomial(k,n-k) * binomial(5*k, k)/(4*k + 1) );
for(n=0, 20, print1(a(n)-b(n), ", "))