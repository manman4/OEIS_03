M=20;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=5, i, j/(j-4)*v[i-j+1]/(i-j)!)); v;
a_vector(M)
