M=21;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, j!*sum(k=0, j\2, (-1)^(j-k)*stirling(j-k, k, 1)/(j-k)!)*binomial(i, j)*v[i-j+1])); v;
a_vector(M)

a(j) = j!*sum(k=0, j\2, (-1)^(j-k)*stirling(j-k, k, 1)/(j-k)!);
for(n=0, 20, print1(a(n), ", "))