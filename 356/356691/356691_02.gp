M=14;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=i*v[i]+i^(2*i)); v;
a_vector(M)