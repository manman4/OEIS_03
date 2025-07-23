M=22;

\\ a(0) = a(1) = 1; a(n) = a(n-1) + a(n-2) + Sum_{k=0..n-1} k * a(k) * a(n-1-k).
a(n) = if(n<2, 1, a(n-1) + a(n-2) + sum(k=0, n-1, k * a(k) * a(n-1-k)));
for(n=0, 12, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1, i, if(i<=2, 1, 0))); for(i=2, n, v[i+1]=v[i]+v[i-1]+sum(j=0, i-1, j*v[j+1]*v[i-j])); v;
for(n=0, M, print(a_vector(n)));
