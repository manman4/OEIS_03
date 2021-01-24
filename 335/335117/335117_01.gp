default(parisize, 12000000000)

n=50000000;
R=vector((n-1)\2,m,2^(valuation(m,2)+3));
forprime(p=3,n,forstep(m=(p-1)/2,#R,(p-1)/2,R[m]*=p^(1+valuation(m,p))));
s=0;
cnt=2;
print(1" "1); print(2" "3); for(k=1,(n-1)\2,t=R[k]; if(t>s, s=t; cnt++; print(cnt" "t+1)))
