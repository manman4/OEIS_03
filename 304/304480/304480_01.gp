L(n)=my(R=vector((n-1)\2,m,2^(valuation(m,2)+3)));forprime(p=3,n,forstep(m=(p-1)/2,#R,(p-1)/2,R[m]*=p^(1+valuation(m,p))));vecmax(R)+1;
a(n) = if(n < 3, 2*n-1, L(n));
for(n=1, 5000, print(n, " ", a(n)))

