M=21;

a308345(n) = n!*sumdiv(n, d, 1/(d*(n/d)^d));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=2, i, j*a308345(j-1)*binomial(i-1, j-1)*v[i-j+1])); v;
a_vector(M)
