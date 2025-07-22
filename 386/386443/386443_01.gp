M=18;

\\ a(0) = 1; a(n) = a(n-1) + Sum_{k=0..n-1} k^2 * a(k) * a(n-1-k).
a(n) = if(n==0, 1, a(n-1) + sum(k=0, n-1, k^2 * a(k) * a(n-1-k)));
for(n=0, 12, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, i-1, j^2*v[j+1]*v[i-j])); v;
a_vector(M)