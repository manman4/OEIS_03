M=22;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=1, (i+1)\3, (3*j-1)/j*v[i-3*j+2]/(i-3*j+1)!)); v;
a_vector(M)