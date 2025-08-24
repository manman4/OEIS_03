a(n) = polcoef(2/(1+2*(n-1)*x+((n+1)*x)^2+(1+(n-1)*x)*sqrt(1+2*(n-1)*x+((n+1)*x)^2)), n);
for(n=0, 15, print1(a(n),", "))  
