b(n) = my(f=sum(k=0, (n+1)^(1/3), x^(k^3))+x*O(x^n)); n*polcoeff(log(f), n);

T(n, k) = if(n==0, 1, -k/n*sum(j=1, n, b(j)*T(n-j, k)));
for(n=0, 11, for(k=0, n, print1(T(k,n-k),", ")))
