M=19;

a356600(n) = n!*sum(k=1, n, sigma(k, 2)/(k*(n-k)!));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, a356600(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)