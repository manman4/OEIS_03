M=12;

a_vector(n) = my(v=vector(n+1)); v[1]=0; for(i=1, n, v[i+1]=1-sum(j=1, i-1, 3^(j*(i-j))*binomial(i-1, j)*v[i-j+1])); v;
a_vector(M)