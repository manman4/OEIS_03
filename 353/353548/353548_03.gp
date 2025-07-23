M=17;

a_vector(n) = my(v=vector(n+1, i, if(i==2, 1, 0))); for(i=2, n, v[i+1]=(4*i-3)*v[i]-4*(i-1)*v[i-1]+1); v;
for(n=0, M, print(a_vector(n)));