M=22;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=1, (i+3)\4, (4*j-3)/j*v[i-4*j+4]/(i-4*j+3)!)); v;
a_vector(M)