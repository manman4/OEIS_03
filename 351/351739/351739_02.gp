M=20;

b(n) = n*sum(k=1, n-1, (k-1)!*abs(stirling(n-1, k, 1)));
for(n=0, 15, print1(b(n),", "))
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, b(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)   