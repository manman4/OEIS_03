M=19;

a354843(n) = n!*sumdiv(n, d, (n/d)^d/d!);
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, a354843(j)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)