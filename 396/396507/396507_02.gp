H4(M)=
{
  if(M<=0, return(0));
  my(q=M\4,r=M%4,s);
  s=-128*q^2*(q-1)^2 - 128*q*(q-1)*(2*q-1) - 208*q*(q-1) - 80*q;
  if(r>=1, s+=(4*q+1)^4);
  if(r>=3, s-=(4*q+3)^4);
  s
};

Cs(M)=if(M<=0,0,if((M%4)==1 || (M%4)==2,1,0));

P2(n)=n*(n+1)*(2*n+1)/6;
P4(n)=n*(n+1)*(2*n+1)*(3*n^2+3*n-1)/30;

SE4(N)=
{
  my(s=0,k=1,q,u);
  while(k<=N,
    q=N\k; u=N\q;
    s += q*(H4(u)-H4(k-1));
    k=u+1
  );
  s
};

SE4p(N)=
{
  my(s=0,k=1,q,u);
  while(k<=N,
    q=N\k; u=N\q;
    s += Cs(q)*(P4(u)-P4(k-1));
    k=u+1
  );
  s
};

T4(N)=
{
  my(L=sqrtint(N),s=0,B,A2,A4);
  for(a=-L,L,
    B=sqrtint(N-a*a);
    A2=a*a; A4=A2*A2;
    s += (2*B+1)*A4 - 12*A2*P2(B) + 2*P4(B)
  );
  s/4
};

a341399(N)=1 + 4*(SE4(N)+16*SE4p(N)+8*T4(N))/5;

a(n)=a341399(10^n);
for(n=0, 31, print1(a341399(n),", "));

\\ for(n=0, 16, write("b373896.txt", n, " ", a(n)));


