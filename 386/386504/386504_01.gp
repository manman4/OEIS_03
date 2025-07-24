M=15;

\\ a(0) = 1; a(n) = a(n-1) + Sum_{k=0..n-1} (1 + k) * (24*k - 50*k^2 + 35*k^3 - 10*k^4 + k^5) * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, a(n-1) + sum(k=0, n-1, (1 + k) * (24*k - 50*k^2 + 35*k^3 - 10*k^4 + k^5) * binomial(n-1,k) * a(k) * a(n-1-k)));
for(n=0, 12, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, i-1, (1+j)*sum(k=1, 5, stirling(5, k, 1)*j^k)*binomial(i-1, j)*v[j+1]*v[i-j])); v;
for(n=0, M, print(a_vector(n)));