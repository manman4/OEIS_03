a_vector(n) = my(v=vector(n)); v[1]=1; for(i=1, n-1, v[i+1]=sum(j=1, i, 2^(i-j)*abs(stirling(i+1, j, 1))*v[j])); v;
a_vector(30)