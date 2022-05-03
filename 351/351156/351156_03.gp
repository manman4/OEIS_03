M=25;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=2, (i+2)\3, (3*j-2)/((j-1)*6^(j-1))*v[i-3*j+3]/(i-3*j+2)!)); v;
a_vector(M)