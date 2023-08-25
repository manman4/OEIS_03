a(n) = n! * sum(k=0, n, k^(n-k) * binomial(n+3*k+1,k)/( (n+3*k+1)*(n-k)! ) );
for(n=0, 16, print1(a(n), ", "))