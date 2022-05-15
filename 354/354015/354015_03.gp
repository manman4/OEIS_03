M=21;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=1, i, (1+2*sum(k=1, j-1, 1/k))*v[i-j+1]/(i-j)!)); v;
a_vector(M)