M=18;

a356539(n) = sumdiv(n, d, d*3^(n-d));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=1, i, a356539(j)*v[i-j+1]/(i-j)!)); v;
a_vector(M)