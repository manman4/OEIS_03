M=18;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*sum(j=0, i-1, abs(stirling(i-1, j, 1))*v[j+1])); v;
a_vector(M)
