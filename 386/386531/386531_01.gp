M=17;

\\ a(0) = 1; a(n) = a(n-1) + (1/6) * Sum_{k=0..n-1} (-k + k^3) * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, a(n-1) + (1/6) * sum(k=0, n-1, (-k + k^3) * binomial(n-1,k) * a(k) * a(n-1-k)));
for(n=0, 12, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, i-1, (1+j)*sum(k=1, 2, stirling(2, k, 1)*j^k)*binomial(i-1, j)*v[j+1]*v[i-j])/6); v;
for(n=0, M, print(a_vector(n)));