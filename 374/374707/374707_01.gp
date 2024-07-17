a(n) = my(v=vector(2*n^3+1)); for(i=0, n, for(j=i, n, v[i^3+j^3+1]+=1)); sum(i=1, #v, v[i]>0);
for(n=0, 55, print1(a(n),", "))                    
