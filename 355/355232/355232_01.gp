M=30;

a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i-1, 2^j*binomial(i, j)*v[j])); concat(0, v);
a_vector(M)