b(n) = my(f=sum(k=0, (n+1)^(1/2), x^(k^2))+x*O(x^n)); n*polcoeff(log(f), n);

a(n) = if(n==0, 1, -2/n*sum(k=1,n, b(k)*a(n-k)));
for(n=0, 20, print1(a(n),", ")) 
