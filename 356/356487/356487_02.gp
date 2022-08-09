M=17;

a356486(n) = (n-1)!*sumdiv(n, d, d^n/(d-1)!);
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, a356486(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)