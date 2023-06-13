a(n) = sumdiv(n, d, (n/d)^d*(d+1));
for(n=1, 43, print1(a(n),", "))  

for(n=1, 43, if(a(n)==1+3*n, print1(n, ", ")))