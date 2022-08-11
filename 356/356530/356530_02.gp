M=18;

a356529(n) = (n-1)!*sumdiv(n, d, d^(n-d+1));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, a356529(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)