M=21;

\\ a(0) = 1; a(n) = 120 * Sum_{k=1..n} binomial(n,k) * Stirling2(k,5) * a(n-k).
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=120*sum(j=1, i, binomial(i, j)*stirling(j, 5, 2)*v[i-j+1])); v;
a_vector(M)
