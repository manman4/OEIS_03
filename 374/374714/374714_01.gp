a(n) = my(v=vector(4*n^3)); for(i=1, n, for(j=i, n, for(k=j, n, for(l=k, n, v[i^3+j^3+k^3+l^3]+=1)))); sum(i=1, #v, v[i]>0);
for(n=1, 47, print1(a(n),", "))                    
