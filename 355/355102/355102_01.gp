M=19;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=2*i*sum(j=0, i-1, (-1)^(i-j-1)*stirling(i-1, j, 2)*v[j+1])); v;
a_vector(M)