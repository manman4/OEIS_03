a(n) = my(v=vector(3*n^3+1)); for(i=0, n, for(j=i, n, for(k=j, n, v[i^3+j^3+k^3+1]+=1))); sum(i=1, #v, v[i]>0);
for(n=0, 50, print1(a(n),", "))                    
