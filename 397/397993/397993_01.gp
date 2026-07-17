a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 2^(i-j)*abs(stirling(i+1, j, 1))*v[j])); v;
a_vector(30)

\\ a(1) = 1; a(n+1) = Sum_{k=1..n} 2^(n-k) * |Stirling1(n+1,k)| * a(k).
a(n) = if(n==1, 1, sum(k=1, n-1, 2^(n-1-k) * abs(stirling(n, k, 1)) * a(k)));
for(n=1, 15, print1(a(n),", "));