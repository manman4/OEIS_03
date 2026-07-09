\\ C(M) = sum(k=1,M, chi4(k))
Cs(M)=if(M<=0,0,if((M%4)==1 || (M%4)==2,1,0));

\\ G(M) = sum(k=1,M, chi4(k)*k^2)
G2(M)=
{
  my(q=M\4,r=M%4);
  if(M<=0,0,
    if(r==0,-8*q^2,
      if(r==1,8*q^2+8*q+1,
        if(r==2,8*q^2+8*q+1,-8*(q+1)^2)
      )
    )
  )
};

sumsq(n)=n*(n+1)*(2*n+1)/6;

B6(N)=
{
  my(s=1,k=1,q,u);
  while(k<=N,
    q=N\k;
    u=N\q;
    s += 16*Cs(q)*(sumsq(u)-sumsq(k-1)) - 4*q*(G2(u)-G2(k-1));
    k=u+1
  );
  s
};

a(n)=B6(10^n);

for(n=0, 14, write("b373884_1.txt", n, " ", a(n)));


