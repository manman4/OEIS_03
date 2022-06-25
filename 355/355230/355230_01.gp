M=30;

a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i-1, 2^(i-j)*(j-1)!*binomial(i, j)*v[i-j])); concat(0, v);
a_vector(M)