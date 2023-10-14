M=21;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=6*sum(j=1, i, binomial(i-1, j-1)*abs(stirling(j, 3, 1))*v[i-j+1])); v;
a_vector(M)