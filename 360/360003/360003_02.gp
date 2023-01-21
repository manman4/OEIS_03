M=83;

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=-sum(j=1, i, sumdiv(j, d, issquare(d-10)*d)*v[i-j+1])/i); v;
a_vector(M)