M=20;

a_vector(n) = my(v=vector(n+1)); v[1]=0; v[2]=1; for(i=2, n, v[i+1]=(i+3)*v[i]-4*(i-1)*v[i-1]+4^(i-1)); v; 
a_vector(M)
