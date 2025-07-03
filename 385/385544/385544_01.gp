M=19;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (n-k)^(k+1) * a(k).
a(n) = if(n==0, 1, sum(k=0, n-1, (n-k)^(k+1) * a(k)));
for(n=0, M, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, (i-j)^(j+1)*v[j+1])); v;
a_vector(M)