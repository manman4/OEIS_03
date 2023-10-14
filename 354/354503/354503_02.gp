M=21;

a354506(n) = n!*sum(k=1, n, sumdiv(k, d, (-1)^(k/d+1))/(k*(n-k)!));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, a354506(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)