M=17;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=3*sum(j=1, i, j*abs(stirling(i, j, 1))*v[j])); v;
a_vector(M)