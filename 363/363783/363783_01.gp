a(n) = my(f=sum(k=0, (n+1)^(1/3), x^(k^3))+x*O(x^n)); n*polcoeff(log(f), n);
for(n=1, 61, print1(a(n),", ")) 

