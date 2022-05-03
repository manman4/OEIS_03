M=22;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=2, (i+1)\2, (2*j-1)/(j-1)*v[i-2*j+2]/(i-2*j+1)!)); v;
a_vector(M)