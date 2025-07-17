\\ a(n) = n + 1 + n * Sum_{k=0..n-1} a(k) * a(n-1-k).
a(n) = n + 1 + n * sum(k=0, n-1, a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "))

\\ a(n) = n + 1 + Sum_{k=0..n-1} (1 + 2*k) * a(k) * a(n-1-k).
a(n) = n + 1 + sum(k=0, n-1, (1 + 2*k) * a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "))

a_vector(n) = my(v=vector(n+1)); for(i=0, n, v[i+1]=i+1+i*sum(j=0, i-1, v[j+1]*v[i-j])); v;
a_vector(18)
