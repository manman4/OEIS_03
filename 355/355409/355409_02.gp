M=18;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, (3^j-2^j)*binomial(i, j)*v[i-j+1])); v;
a_vector(M)