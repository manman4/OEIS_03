a(n) = my(f=sum(k=0, (n+1)^(1/4), x^(k^4))+x*O(x^n)); n*polcoeff(log(f), n);
for(n=1, 62, print1(a(n),", ")) 

