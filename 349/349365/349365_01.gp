a(n) = if(n==0, 1, a(n-1) + sum(k=0, (n-1)\2, a(k) * a(n-1-2*k)));
for(n=0, 15, print1(a(n),", "))   

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=v[i]+sum(j=0, (i-1)\2, v[j+1]*v[i-2*j])); v; 
a_vector(30)
