M=24;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, binomial(i+1-j, j)*v[i-j])); v;
a_vector(M)