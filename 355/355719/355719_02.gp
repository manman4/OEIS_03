M=22;

a006252(n) = sum(k=0, n, k!*stirling(n, k, 1));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, j*a006252(j-1)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)