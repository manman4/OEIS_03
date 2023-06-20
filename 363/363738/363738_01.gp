a(n) = if(n==0, 1, sum(k=2, n, ispower(k,3)*a(n-k)));
for(n=0, 35, print1(a(n),", "))  

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=2, i, ispower(j, 3)*v[i-j+1])); v;
a_vector(107)
