M=14;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, j*2^(j-1)*stirling(i, j, 1)*v[j])); v;
a_vector(M)