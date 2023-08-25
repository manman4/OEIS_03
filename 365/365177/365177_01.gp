a(n) = n! * sum(k=0, n, k^(n-k) * binomial(3*n+k+1,k)/( (3*n+k+1)*(n-k)! ) );
for(n=0, 16, print1(a(n), ", "))