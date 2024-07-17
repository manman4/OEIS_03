a(n) = my(v=vector(3*n^2)); for(i=1, n, for(j=i, n, for(k=j, n, v[i^2+j^2+k^2]+=1))); sum(i=1, #v, v[i]>0);
for(n=1, 54, print1(a(n),", "))                    
