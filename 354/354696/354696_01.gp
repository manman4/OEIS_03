M=32;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\4, binomial(i+2, 4*j+3)*v[j+1])); v;
a_vector(M)