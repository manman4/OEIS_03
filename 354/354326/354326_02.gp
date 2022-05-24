M=20;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=2, i, j*4^(j-2)*binomial(i, j)*v[i-j+1])/2); v;
a_vector(M)