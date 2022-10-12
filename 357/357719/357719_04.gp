M=21;

a_vector(n) = my(v=vector(n+1)); v[1]=1; v[2]=0; for(i=2, n, v[i+1]=-(2*i-3)*v[i]-(i^2-4*i+8)*v[i-1]); v;
a_vector(M)