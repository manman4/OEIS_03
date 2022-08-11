M=19;

a090879(n) = sumdiv(n, d, d*2^(n-d));
a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=(i-1)!*sum(j=1, i, a090879(j)*v[i-j+1]/(i-j)!)); v;
a_vector(M)